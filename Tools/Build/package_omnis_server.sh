#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
OUTPUT_DIR="$ROOT/Build/OmnisServer"
LOG_FILE="$ROOT/Saved/Logs/omnis_ue56_server_package.log"

: "${UE_ENGINE_ROOT:?Set UE_ENGINE_ROOT to the Unreal Engine 5.8 installation on the dedicated runner}"
[[ -d "$UE_ENGINE_ROOT/Engine" ]] || { echo "Invalid UE_ENGINE_ROOT: $UE_ENGINE_ROOT" >&2; exit 1; }
[[ -f "$PROJECT_FILE" ]] || { echo "Omnis.uproject not found: $PROJECT_FILE" >&2; exit 1; }

RUN_UBT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUBT.sh"
RUN_UAT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
[[ -x "$RUN_UBT" ]] || { echo "RunUBT.sh not found: $RUN_UBT" >&2; exit 1; }
[[ -x "$RUN_UAT" ]] || { echo "RunUAT.sh not found: $RUN_UAT" >&2; exit 1; }
mkdir -p "$(dirname "$LOG_FILE")" "$OUTPUT_DIR"

echo "=== OMNIS UE 5.8 REAL DEDICATED SERVER PACKAGE ===" | tee "$LOG_FILE"
echo "Commit: ${CI_COMMIT_SHA:-local}" | tee -a "$LOG_FILE"
echo "UE_ENGINE_ROOT: $UE_ENGINE_ROOT" | tee -a "$LOG_FILE"

"$RUN_UBT" OmnisServer Linux Development "-Project=$PROJECT_FILE" -WaitMutex 2>&1 | tee -a "$LOG_FILE"

SERVER_BINARY="$(find "$ROOT/Binaries" -type f -iname 'OmnisServer' -print -quit 2>/dev/null || true)"
[[ -n "$SERVER_BINARY" ]] || { echo "OmnisServer binary was not produced by UBT." | tee -a "$LOG_FILE" >&2; exit 1; }

echo ">>> Cook/stage/package dedicated server" | tee -a "$LOG_FILE"
rm -rf "$OUTPUT_DIR"
"$RUN_UAT" BuildCookRun \
  -project="$PROJECT_FILE" \
  -noP4 \
  -unattended \
  -utf8output \
  -build \
  -server \
  -serverconfig=Development \
  -noclient \
  -cook \
  -stage \
  -pak \
  -map="/Game/Omnis/Maps/Omnis_OpenWorld" \
  -archive \
  -archivedirectory="$OUTPUT_DIR" 2>&1 | tee -a "$LOG_FILE"

find "$OUTPUT_DIR" -maxdepth 4 -type f -print | sort | tee -a "$LOG_FILE"
if ! find "$OUTPUT_DIR" -maxdepth 5 -type f -perm -u+x -iname 'OmnisServer*' -print -quit 2>/dev/null | grep -q .; then
  echo "Packaged dedicated server executable was not found under $OUTPUT_DIR." | tee -a "$LOG_FILE" >&2
  exit 1
fi

echo "=== RESULT: REAL UE 5.8 DEDICATED SERVER PACKAGE PASSED ===" | tee -a "$LOG_FILE"
