#!/usr/bin/env python3
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PLATFORMS = {"Win64", "PS5", "XSX", "Switch"}


def fail(message: str) -> None:
    print(f"[FAIL] {message}")
    raise SystemExit(1)


def main() -> None:
    platform = os.environ.get("OMNIS_PLATFORM", "")
    if platform not in PLATFORMS:
        fail("OMNIS_PLATFORM must be one of Win64, PS5, XSX, Switch")

    if not os.environ.get("UE_ENGINE_ROOT"):
        fail("UE_ENGINE_ROOT is required for a real release candidate build")

    if platform in {"PS5", "XSX", "Switch"} and os.environ.get("OMNIS_CONSOLE_SDK_READY") != "true":
        fail("authorized console SDK readiness is required; no SDK simulation is accepted")

    readiness_path = ROOT / "Content/Omnis/Data/release-readiness.json"
    if not readiness_path.is_file():
        fail("Content/Omnis/Data/release-readiness.json is missing")

    readiness = json.loads(readiness_path.read_text(encoding="utf-8"))
    if readiness.get("status") != "READY":
        fail("release-readiness status is not READY; final content/hardware gates remain open")

    for required in ("Saved/Release/provenance.json", "Saved/Release/crash-evidence.json"):
        if not (ROOT / required).is_file():
            fail(f"{required} must be generated before RC validation")

    audit_path = ROOT / "Saved/Release/content-delivery-audit.json"
    if not audit_path.is_file():
        fail("content-delivery-audit.json must be generated before RC validation")

    audit = json.loads(audit_path.read_text(encoding="utf-8"))
    if audit.get("status") != "READY":
        fail("content delivery audit is not READY; repository evidence is incomplete")

    print("[PASS] release candidate gate satisfied.")


if __name__ == "__main__":
    main()
