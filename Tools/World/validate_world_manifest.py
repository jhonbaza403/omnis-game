#!/usr/bin/env python3
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[2]
manifest = ROOT / "Content/Omnis/World/world-manifest.json"

def main() -> None:
    data = json.loads(manifest.read_text(encoding="utf-8"))
    assert data["cell_size"] > 0
    assert isinstance(data["regions"], list) and data["regions"]
    print("[PASS] world manifest is valid.")

if __name__ == "__main__":
    main()
