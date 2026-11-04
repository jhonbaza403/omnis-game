#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
if [[ -z "${UE_ENGINE_ROOT:-}" ]]; then
  echo "UE_ENGINE_ROOT must point to Unreal Engine 5.8." >&2
  exit 1
fi

EDITOR="$UE_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd"
RUN_ID="${CI_JOB_ID:-$(date -u +%Y%m%dT%H%M%SZ)}"
LOG_FILE="$ROOT/Saved/Logs/omnis_unreal_automation_$RUN_ID.log"
REPORT_DIR="$ROOT/Saved/Automation/$RUN_ID"

[[ -x "$EDITOR" ]] || { echo "UnrealEditor-Cmd not found: $EDITOR" >&2; exit 1; }
[[ -f "$PROJECT_FILE" ]] || { echo "Omnis.uproject not found: $PROJECT_FILE" >&2; exit 1; }

mkdir -p "$REPORT_DIR" "$(dirname "$LOG_FILE")"

"$EDITOR" "$PROJECT_FILE" \
  -unattended \
  -nop4 \
  -nosplash \
  -stdout \
  -AllowStdOutLogVerbosity \
  -nullrhi \
  -ExecCmds="Automation RunTests Omnis; Quit" \
  -TestExit="Automation Test Queue Empty" \
  -ReportExportPath="$REPORT_DIR" \
  -AbsLog="$LOG_FILE" 2>&1 | tee "$LOG_FILE"

grep -q "Omnis.Experience.Systems" "$LOG_FILE" || {
  echo "Native Omnis Unreal automation test did not execute." >&2
  exit 1
}

test -d "$REPORT_DIR"
test -n "$(find "$REPORT_DIR" -type f -print -quit)"

echo "=== RESULT: REAL OMNIS UE5.8 AUTOMATION PASSED ==="
echo "Log: $LOG_FILE"
echo "Report: $REPORT_DIR"
