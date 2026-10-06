#!/usr/bin/env python3
"""Audit real Omnis release-content evidence without inventing assets or approvals."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "Content/Omnis/Data/content-delivery-contract.json"
CONTENT_ROOT = ROOT / "Content/Omnis"
LOCALIZATION_ROOT = ROOT / "Config/Localization"
REPORT = ROOT / "Saved/Release/content-delivery-audit.json"

EXTENSIONS = {
    ".uasset", ".umap", ".wav", ".ogg", ".flac", ".fbx", ".gltf", ".glb",
    ".png", ".jpg", ".jpeg", ".tga", ".exr", ".po", ".locres", ".json", ".ini",
}


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


def load_contract() -> dict[str, Any]:
    return json.loads(CONTRACT.read_text(encoding="utf-8"))


def audit() -> dict[str, Any]:
    contract = load_contract()
    deliverables = contract["deliverables"]
    results: dict[str, Any] = {}

    for key, spec in deliverables.items():
        evidence = files_under(
            spec["requiredPaths"],
            spec["requiredExtensions"],
        )
        results[key] = {
            "physicalEvidence": len(evidence) > 0,
            "evidenceFiles": evidence,
            "acceptance": spec["acceptance"],
            "gate": "OPEN",
        }

    # Localization already has a runtime contract in the repository. Presence alone
    # is deliberately insufficient for a final gate because linguistic QA is external.
    localization_contract = ROOT / "Tools/Validation/validate_localization_contract.py"
    results["localization"]["runtimeContractPresent"] = localization_contract.is_file()
    results["localization"]["gate"] = "OPEN"

    hardware = {
        "PS5": "OPEN",
        "XboxSeries": "OPEN",
        "Switch2": "OPEN",
        "PCReference": "OPEN",
    }
    platform = {
        "PSN": "OPEN",
        "Xbox": "OPEN",
        "Nintendo": "OPEN",
    }
    store = {
        "storeSubmission": "OPEN",
        "distributionSigning": "OPEN",
        "trcTcrCertification": "OPEN",
    }

    closed = [
        all(item["physicalEvidence"] for item in results.values()
            if item["gate"] == "CLOSED")
        and bool(results)
    ]
    status = "BLOCKED" if not closed else "READY"

    report = {
        "schema": "omnis.content-delivery-audit.v1",
        "principle": contract["principle"],
        "noFakeEvidence": contract["noFakeEvidence"],
        "status": status,
        "deliverables": results,
        "hardwareEvidence": hardware,
        "platformIntegration": platform,
        "storeAndCertification": store,
    }
    return report


def main() -> None:
    report = audit()
    REPORT.parent.mkdir(parents=True, exist_ok=True)
    REPORT.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    print("[CONTENT-AUDIT] status=" + report["status"])
    for key, value in report["deliverables"].items():
        print(
            f"[CONTENT-AUDIT] {key}: "
            f"physicalEvidence={value['physicalEvidence']} "
            f"files={len(value['evidenceFiles'])}"
        )
    print("[CONTENT-AUDIT] report=" + REPORT.relative_to(ROOT).as_posix())


if __name__ == "__main__":
    main()
