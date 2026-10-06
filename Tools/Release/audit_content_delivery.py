#!/usr/bin/env python3
"""Audit real Omnis release evidence without inventing assets, approvals, or hardware results."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "Content/Omnis/Data/content-delivery-contract.json"
REPORT = ROOT / "Saved/Release/content-delivery-audit.json"


def files_under(paths: list[str], extensions: list[str]) -> list[str]:
    allowed = {item.lower() for item in extensions}
    found: list[str] = []
    for raw in paths:
        base = ROOT / raw
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if path.is_file() and path.suffix.lower() in allowed:
                found.append(path.relative_to(ROOT).as_posix())
    return sorted(found)


def exists(path: str) -> bool:
    return (ROOT / path).is_file()


def load_contract() -> dict[str, Any]:
    return json.loads(CONTRACT.read_text(encoding="utf-8"))


def audit() -> dict[str, Any]:
    contract = load_contract()
    if contract.get("noFakeEvidence") is not True:
        raise ValueError("content delivery contract must enforce noFakeEvidence=true")

    deliverables: dict[str, Any] = {}
    for key, spec in contract["deliverables"].items():
        evidence = files_under(spec["requiredPaths"], spec["requiredExtensions"])
        qa_path = spec["qaEvidencePath"]
        physical = len(evidence) > 0
        qa = exists(qa_path)
        deliverables[key] = {
            "physicalEvidence": physical,
            "qaEvidencePresent": qa,
            "evidenceFiles": evidence,
            "qaEvidencePath": qa_path,
            "acceptance": spec["acceptance"],
            "gate": "CLOSED" if physical and qa else "OPEN",
        }

    hardware: dict[str, Any] = {}
    for platform, spec in contract["hardwareEvidence"].items():
        present = exists(spec["evidencePath"])
        hardware[platform] = {
            "evidencePresent": present,
            "evidencePath": spec["evidencePath"],
            "gate": "CLOSED" if present else "OPEN",
            "requirement": spec["requirement"],
        }

    platform: dict[str, Any] = {}
    for provider, spec in contract["platformIntegration"].items():
        present = exists(spec["evidencePath"])
        platform[provider] = {
            "evidencePresent": present,
            "evidencePath": spec["evidencePath"],
            "gate": "CLOSED" if present else "OPEN",
            "requirement": spec["requirement"],
        }

    store: dict[str, Any] = {}
    for gate, spec in contract["storeAndCertification"].items():
        present = exists(spec["evidencePath"])
        store[gate] = {
            "evidencePresent": present,
            "evidencePath": spec["evidencePath"],
            "gate": "CLOSED" if present else "OPEN",
            "requirement": spec["requirement"],
        }

    all_closed = (
        all(item["gate"] == "CLOSED" for item in deliverables.values())
        and all(item["gate"] == "CLOSED" for item in hardware.values())
        and all(item["gate"] == "CLOSED" for item in platform.values())
        and all(item["gate"] == "CLOSED" for item in store.values())
    )

    return {
        "schema": "omnis.content-delivery-audit.v2",
        "principle": contract["principle"],
        "noFakeEvidence": True,
        "status": "READY" if all_closed else "BLOCKED",
        "deliverables": deliverables,
        "hardwareEvidence": hardware,
        "platformIntegration": platform,
        "storeAndCertification": store,
    }


def main() -> None:
    report = audit()
    REPORT.parent.mkdir(parents=True, exist_ok=True)
    REPORT.write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )

    print("[CONTENT-AUDIT] status=" + report["status"])
    for key, value in report["deliverables"].items():
        print(
            f"[CONTENT-AUDIT] {key}: "
            f"physicalEvidence={value['physicalEvidence']} "
            f"qaEvidencePresent={value['qaEvidencePresent']} "
            f"files={len(value['evidenceFiles'])} "
            f"gate={value['gate']}"
        )
    for key, value in report["hardwareEvidence"].items():
        print(f"[CONTENT-AUDIT] hardware {key}: gate={value['gate']}")
    for key, value in report["platformIntegration"].items():
        print(f"[CONTENT-AUDIT] provider {key}: gate={value['gate']}")
    for key, value in report["storeAndCertification"].items():
        print(f"[CONTENT-AUDIT] release {key}: gate={value['gate']}")
    print("[CONTENT-AUDIT] report=" + REPORT.relative_to(ROOT).as_posix())


if __name__ == "__main__":
    main()
