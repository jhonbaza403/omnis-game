#!/usr/bin/env bash
set -eo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
UE_ENGINE_ROOT="$(printenv UE_ENGINE_ROOT || true)"

if [[ -z "$UE_ENGINE_ROOT" ]]; then
  echo "UE_ENGINE_ROOT must point to Unreal Engine 5.6 on the dedicated runner." >&2
  exit 1
fi

EDITOR="$UE_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd"
LOG_FILE="$ROOT/Saved/Logs/omnis_unreal_automation.log"

if [[ ! -x "$EDITOR" ]]; then
  echo "UnrealEditor-Cmd not found or not executable: $EDITOR" >&2
  exit 1
fi

mkdir -p "$(dirname "$LOG_FILE")"

"$EDITOR" "$PROJECT_FILE"   -unattended   -nop4   -nosplash   -nullrhi   -ExecCmds="Automation RunTests Omnis; Quit"   -TestExit="Automation Test Queue Empty"   2>&1 | tee "$LOG_FILE"

echo "=== Omnis Unreal automation suite completed ==="
