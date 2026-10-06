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
        "Tools/Build/package_platform.sh",
        ["UE_ENGINE_ROOT", "OMNIS_PLATFORM", "PS5", "XSX", "Switch", "BuildCookRun", "-pak"],
    )
    require_text(
        "Docs/PLATFORM_BUILD_RUNBOOK.md",
        ["one gameplay codebase", "Fail-closed behavior", "fake SDK"],
    )
    print("[PASS] real platform packaging contract validated.")

if __name__ == "__main__":
    main()
