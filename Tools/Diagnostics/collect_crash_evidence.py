#!/usr/bin/env python3
import argparse
import hashlib
import json
import os
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEARCH_ROOTS = (ROOT / "Saved/Crashes", ROOT / "Saved/Logs")

def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    entries = []
    for root in SEARCH_ROOTS:
        if not root.exists():
            continue
        for path in sorted(root.rglob("*")):
            if path.is_file():
                entries.append({
                    "path":path.relative_to(ROOT).as_posix(),
                    "bytes":path.stat().st_size,
                    "sha256":sha256(path)
                })

    report = {
        "schema":"omnis.crash-evidence.v1",
        "generatedAt":datetime.now(timezone.utc).isoformat(),
        "gitCommit":os.environ.get("CI_COMMIT_SHA",""),
        "platform":os.environ.get("OMNIS_PLATFORM","unknown"),
        "crashFiles":entries,
        "status":"evidence-present" if entries else "no-crash-files-present"
    }

    output = ROOT / args.output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"[PASS] crash evidence manifest written: {output.relative_to(ROOT)}")

if __name__ == "__main__":
    main()
