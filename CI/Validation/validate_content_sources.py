#!/usr/bin/env python3
"""Static validation for Omnis authored source-content families."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[2]
catalog_path = ROOT / "Content/Omnis/Data/world-content-catalog.json"
source_root = ROOT / "Content/Omnis/SourceAssets"

data = json.loads(catalog_path.read_text(encoding="utf-8"))

family_dirs = {
    "characters": "Characters",
    "animals": "Animals",
    "nature": "Nature",
    "architecture": "Architecture",
    "interiors": "Interiors",
    "vehicles": "Vehicles",
    "aviation": "Aviation",
    "infrastructure": "Infrastructure",
    "tools": "Tools",
}

missing = []
validated = 0

for family, folder in family_dirs.items():
    for name in data["families"].get(family, []):
        path = source_root / folder / (name + ".obj")
        if not path.is_file():
            missing.append(str(path.relative_to(ROOT)))
        else:
            validated += 1

if missing:
    raise SystemExit("Missing authored source meshes:\n" + "\n".join(missing))

print("Source content validation passed.")
print("Meshes validated:", validated)
print("Families validated:", ", ".join(family_dirs))
