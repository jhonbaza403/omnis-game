#!/usr/bin/env python3
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[2]
catalog = ROOT / "Content/Omnis/Data/catalog.json"

def main() -> int:
    data = json.loads(catalog.read_text(encoding="utf-8"))
    identifiers = []
    for section in ("items", "characters"):
        for entry in data.get(section, []):
            identifier = entry.get("id")
            if not identifier or identifier in identifiers:
                print("[FAIL] invalid or duplicate content id")
                return 1
            identifiers.append(identifier)
    print(f"[PASS] {len(identifiers)} content identifiers validated.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
