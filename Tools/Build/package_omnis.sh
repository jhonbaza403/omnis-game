#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
UE_ENGINE_ROOT="${UE_ENGINE_ROOT:-}"
PLATFORM="${UE_TARGET_PLATFORM:-Linux}"
CONFIGURATION="${UE_BUILD_CONFIGURATION:-Development}"
ARCHIVE_DIR="${UE_ARCHIVE_DIR:-$ROOT/Build/Omnis/$PLATFORM}"
[[ -n "$UE_ENGINE_ROOT" ]] || { echo "UE_ENGINE_ROOT must point to Unreal Engine 5.8." >&2; exit 1; }
[[ -x "$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh" ]] || { echo "RunUAT.sh not found." >&2; exit 1; }
[[ -f "$PROJECT_FILE" ]] || { echo "Omnis.uproject not found." >&2; exit 1; }
VERSION_FILE="$UE_ENGINE_ROOT/Engine/Build/Build.version"
python3 - "$VERSION_FILE" <<'PY'
import json,sys
from pathlib import Path
v=json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
assert (v.get("MajorVersion"),v.get("MinorVersion"))==(5,8),v
print(f"Packaging with Unreal Engine {v['MajorVersion']}.{v['MinorVersion']}.{v['PatchVersion']}")
PY
UAT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
rm -rf "$ARCHIVE_DIR"
mkdir -p "$ARCHIVE_DIR" "$ROOT/Saved/Logs"
"$UAT" BuildCookRun -project="$PROJECT_FILE" -noP4 -unattended -utf8output -build -cook -stage -package -archive -pak -targetplatform="$PLATFORM" -clientconfig="$CONFIGURATION" -archivedirectory="$ARCHIVE_DIR" 2>&1 | tee "$ROOT/Saved/Logs/omnis_ue58_client_package.log"
CLIENT_BINARY="$(find "$ARCHIVE_DIR" -maxdepth 6 -type f -perm -u+x -iname 'Omnis*' -print -quit 2>/dev/null || true)"
[[ -n "$CLIENT_BINARY" ]] || { echo "Packaged Omnis client executable was not found." >&2; exit 1; }
echo "=== RESULT: REAL UE 5.8 CLIENT PACKAGE PASSED ==="
echo "Client: $CLIENT_BINARY"
echo "Archive: $ARCHIVE_DIR"
