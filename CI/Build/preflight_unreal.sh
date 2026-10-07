#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"

if [[ -z "$UE_ENGINE_ROOT" ]]; then
  echo "UE_ENGINE_ROOT must point to Unreal Engine 5.6.1." >&2
  exit 1
fi

RUN_UBT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUBT.sh"
RUN_UAT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
EDITOR="$UE_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd"
VERSION_FILE="$UE_ENGINE_ROOT/Engine/Build/Build.version"

test -f "$PROJECT_FILE"
test -x "$RUN_UBT"
test -x "$RUN_UAT"
test -x "$EDITOR"
test -f "$VERSION_FILE"

python3 - "$VERSION_FILE" <<'PY'
import json
import sys
from pathlib import Path

version = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
major = version.get("MajorVersion")
minor = version.get("MinorVersion")
patch = version.get("PatchVersion")
assert (major, minor) == (5, 6), (
    f"Expected UE 5.6.x, detected {major}.{minor}.{patch}"
)
print(f"Unreal Engine {major}.{minor}.{patch} detected")
PY

echo "Project: $PROJECT_FILE"
echo "Engine: $UE_ENGINE_ROOT"
echo "=== RESULT: REAL UE 5.6 PREFLIGHT PASSED ==="
