from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[2]
catalog = ROOT / "Content/Omnis/Data/catalog.json"

def validate_unique_ids(entries, seen):
    for entry in entries:
        identifier = entry.get("id")
        if not identifier or identifier in seen:
            raise ValueError("invalid or duplicate content id")
        seen.add(identifier)

def main() -> int:
    data = json.loads(catalog.read_text(encoding="utf-8"))
    identifiers = set()
    validate_unique_ids(data.get("items", []), identifiers)
    validate_unique_ids(data.get("characters", []), identifiers)
    validate_unique_ids(data.get("achievements", []), identifiers)

    achievements = data.get("achievements", [])
    for achievement in achievements:
        for field in ("name", "description"):
            if not isinstance(achievement.get(field), str) or not achievement[field].strip():
                print(f"[FAIL] achievement {achievement.get('id')} missing {field}")
                return 1

    print(
        f"[PASS] content identifiers validated: "
        f"items={len(data.get('items', []))}, "
        f"characters={len(data.get('characters', []))}, "
        f"achievements={len(achievements)}."
    )
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
