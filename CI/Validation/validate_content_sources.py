#!/usr/bin/env python3
"""Static validation for Omnis authored source-content families."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[2]
catalog = ROOT / "Content/Omnis/Data/world-content-catalog.json"
source_root = ROOT / "Content/Omnis/SourceAssets"

data = json.loads(catalog.read_text(encoding="utf-8"))
missing = []

for family, names in data["families"].items():
    if family not in {"characters","animals","nature","architecture","vehicles","aviation","infrastructure","tools"}:
        continue
    for name in names:
        # Character Wanderer already exists under Characters; other families map directly.
        folder = "Characters" if family == "characters" else family.title()
        path = source_root / folder / (name + ".obj")
        if not path.is_file():
            missing.append(str(path.relative_to(ROOT)))

if missing:
    raise SystemExit("Missing authored source meshes:\\n" + "\\n".join(missing))

print("Source content validation passed.")
print("Families validated:", ", ".join(sorted(k for k in data["families"] if k in {"characters","animals","nature","architecture","vehicles","aviation","infrastructure","tools"})))
