#!/usr/bin/env python3
from pathlib import Path
import json
import sys

ROOT = Path(__file__).resolve().parents[2]
REQUIRED_FILES = [
    "README.md", "LICENSE", ".gitignore", ".gitattributes", ".editorconfig",
    "Omnis.uproject", "Config/DefaultEngine.ini", "Config/DefaultGame.ini",
    "Config/DefaultInput.ini", "Config/DefaultEditor.ini",
    "Source/Omnis/Omnis.Build.cs", "Source/Omnis/Omnis.cpp", "Source/Omnis/Omnis.h",
]
REQUIRED_DIRS = [
    "Source/Omnis/Core", "Source/Omnis/Gameplay", "Source/Omnis/World",
    "Source/Omnis/Narrative", "Source/Omnis/AI", "Source/Omnis/Multiplayer",
    "Source/Omnis/Online", "Source/Omnis/UI", "Source/Omnis/Audio",
    "Source/Omnis/Camera", "Source/Omnis/Data", "Source/Omnis/Systems",
    "Source/Omnis/Developer", "Content/Omnis", "Plugins", "Tools", "Tests", "Docs", "CI",
]

def fail(msg: str) -> None:
    print(f"[FAIL] {msg}")
    sys.exit(1)

def main() -> None:
    missing = [p for p in REQUIRED_FILES if not (ROOT / p).is_file()]
    if missing:
        fail("missing files: " + ", ".join(missing))

    for directory in REQUIRED_DIRS:
        path = ROOT / directory
        if not path.is_dir():
            fail(f"missing directory: {directory}")
        tracked = [p for p in path.rglob("*") if p.is_file()]
        if directory.startswith("Source/Omnis") and not tracked:
            fail(f"source module has no implementation: {directory}")

    placeholders = list(ROOT.rglob(".gitkeep"))
    if placeholders:
        fail("placeholder remains: " + ", ".join(str(p.relative_to(ROOT)) for p in placeholders))

    project = json.loads((ROOT / "Omnis.uproject").read_text(encoding="utf-8"))
    if project.get("EngineAssociation") != "5.6":
        fail("Omnis.uproject must target Unreal Engine 5.6")

    print("[PASS] Omnis repository validation complete.")

if __name__ == "__main__":
    main()
