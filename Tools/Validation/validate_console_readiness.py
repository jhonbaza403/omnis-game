#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def require_text(path: str, fragments: list[str]) -> None:
    content = (ROOT / path).read_text(encoding="utf-8")
    missing = [fragment for fragment in fragments if fragment not in content]
    if missing:
        raise AssertionError(f"{path}: missing {missing}")

def main() -> None:
    require_text(
        "Config/DefaultInput.ini",
        ["Gamepad_LeftY", "Gamepad_LeftX", "Gamepad_RightX", "Gamepad_RightY"],
    )
    require_text(
        "Source/Omnis/UI/Accessibility/OmnisAccessibilitySubsystem.h",
        ["SetSubtitleScale", "SetSubtitleMode", "SetHighContrastUI", "SetReduceMotion"],
    )
    require_text(
        "Docs/PLAYSTATION_CERTIFICATION_READINESS.md",
        ["P0 — Plataforma y SDK", "P4 — Submission", "60 Hz"],
    )
    require_text(
        "Docs/AAA_PRODUCTION_STANDARD.md",
        ["Release Candidate", "hardware real"],
    )
    print("[PASS] console/AAA readiness contracts validated.")

if __name__ == "__main__":
    main()
