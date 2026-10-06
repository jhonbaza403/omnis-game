#!/usr/bin/env python3
import argparse
import hashlib
import json
import os
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXCLUDED_DIRS = {".git","Binaries","DerivedDataCache","Intermediate","Saved","node_modules","__pycache__"}

def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()

def iter_files():
    for path in sorted(ROOT.rglob("*")):
        if not path.is_file():
            continue
        rel = path.relative_to(ROOT)
        if any(part in EXCLUDED_DIRS for part in rel.parts):
            continue
        yield path, rel.as_posix()

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True)
    parser.add_argument("--artifact", default=None)
    args = parser.parse_args()

    project = json.loads((ROOT / "Omnis.uproject").read_text(encoding="utf-8"))
    files = [{"path":rel,"bytes":path.stat().st_size,"sha256":sha256(path)} for path, rel in iter_files()]

    provenance = {
        "schema":"omnis.source-provenance.v1",
        "generatedAt":datetime.now(timezone.utc).isoformat(),
        "git":{
            "commit":os.environ.get("CI_COMMIT_SHA",""),
            "branch":os.environ.get("CI_COMMIT_REF_NAME",""),
            "pipelineId":os.environ.get("CI_PIPELINE_ID","")
        },
        "project":{
            "name":project.get("ProjectName","Omnis: El Último Ciclo"),
            "engineAssociation":project.get("EngineAssociation"),
            "modules":[module.get("Name") for module in project.get("Modules",[])],
            "plugins":[plugin.get("Name") for plugin in project.get("Plugins",[])]
        },
        "files":files
    }

    if args.artifact:
        artifact = Path(args.artifact)
        if not artifact.is_file():
            raise SystemExit(f"artifact not found: {artifact}")
        provenance["artifact"] = {
            "path":artifact.as_posix(),
            "bytes":artifact.stat().st_size,
            "sha256":sha256(artifact)
        }

    output = ROOT / args.output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(provenance, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"[PASS] provenance written: {output.relative_to(ROOT)}")

if __name__ == "__main__":
    main()
