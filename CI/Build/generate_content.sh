#!/usr/bin/env bash
set -eo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
SCRIPT_FILE="$ROOT/Tools/Editor/GenerateOmnisVerticalSlice.py"
UE_ENGINE_ROOT="$(printenv UE_ENGINE_ROOT || true)"

if [[ -z "$UE_ENGINE_ROOT" ]]; then
  echo "UE_ENGINE_ROOT must point to Unreal Engine 5.6 on the dedicated runner." >&2
  exit 1
fi

EDITOR="$UE_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd"
LOG_FILE="$ROOT/Saved/Logs/omnis_content_generation.log"
MAP_FILE="$ROOT/Content/Omnis/Maps/Omnis_VerticalSlice.umap"

if [[ ! -x "$EDITOR" ]]; then
  echo "UnrealEditor-Cmd not found or not executable: $EDITOR" >&2
  exit 1
fi

if [[ ! -f "$PROJECT_FILE" || ! -f "$SCRIPT_FILE" ]]; then
  echo "Project or editor generator is missing." >&2
  exit 1
fi

mkdir -p "$(dirname "$LOG_FILE")"
rm -f "$MAP_FILE"

"$EDITOR" "$PROJECT_FILE"   -unattended   -nop4   -nosplash   -nullrhi   -ExecutePythonScript="$SCRIPT_FILE"   2>&1 | tee "$LOG_FILE"

if [[ ! -s "$MAP_FILE" ]]; then
  echo "Unreal did not generate the persistent vertical slice map: $MAP_FILE" >&2
  exit 1
fi

echo "=== REAL CONTENT GENERATION PASSED ==="
echo "Generated map: $MAP_FILE"
