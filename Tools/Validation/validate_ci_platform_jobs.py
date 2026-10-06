#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CI = (ROOT / ".gitlab-ci.yml").read_text(encoding="utf-8")

def main() -> None:
    for fragment in (
        "package_win64:",
        "package_playstation5:",
        "package_xbox_series:",
        "package_nintendo_switch:",
        "OMNIS_PLATFORM: "Win64"",
        "OMNIS_PLATFORM: "PS5"",
        "OMNIS_PLATFORM: "XSX"",
        "OMNIS_PLATFORM: "Switch"",
        "OMNIS_CONSOLE_SDK_READY: "true"",
    ):
        if fragment not in CI:
            raise AssertionError(f".gitlab-ci.yml: missing {fragment}")
    print("[PASS] platform package jobs validated.")

if __name__ == "__main__":
    main()
