#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
LOG_FILE="$ROOT/Saved/Logs/omnis_ue56_build.log"

: "${UE_ENGINE_ROOT:?Set UE_ENGINE_ROOT to the Unreal Engine 5.6 installation on the dedicated runner}"

if [[ ! -d "$UE_ENGINE_ROOT/Engine" ]]; then
  echo "UE_ENGINE_ROOT does not contain an Unreal Engine installation: $UE_ENGINE_ROOT" >&2
  exit 1
fi

if [[ ! -f "$PROJECT_FILE" ]]; then
  echo "Omnis.uproject not found: $PROJECT_FILE" >&2
  exit 1
fi

mkdir -p "$(dirname "$LOG_FILE")"

VERSION_FILE="$UE_ENGINE_ROOT/Engine/Build/Build.version"
if [[ ! -f "$VERSION_FILE" ]]; then
  echo "Unreal Engine Build.version not found: $VERSION_FILE" >&2
  exit 1
fi

UE_MAJOR="$(sed -n 's/.*"MajorVersion"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "$VERSION_FILE" | head -n 1)"
UE_MINOR="$(sed -n 's/.*"MinorVersion"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "$VERSION_FILE" | head -n 1)"
UE_PATCH="$(sed -n 's/.*"PatchVersion"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "$VERSION_FILE" | head -n 1)"

if [[ "$UE_MAJOR.$UE_MINOR" != "5.6" ]]; then
  echo "Wrong Unreal Engine version: detected $UE_MAJOR.$UE_MINOR.$UE_PATCH; required 5.6.x" >&2
  exit 1
fi

RUN_UBT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUBT.sh"
if [[ ! -x "$RUN_UBT" ]]; then
  echo "RunUBT.sh not found or not executable: $RUN_UBT" >&2
  exit 1
fi

echo "=== OMNIS UE 5.6 REAL BUILD ===" | tee "$LOG_FILE"
echo "Commit: ${CI_COMMIT_SHA:-local}" | tee -a "$LOG_FILE"
echo "Runner: ${CI_RUNNER_DESCRIPTION:-local}" | tee -a "$LOG_FILE"
echo "UE_ENGINE_ROOT: $UE_ENGINE_ROOT" | tee -a "$LOG_FILE"
echo "UE version: $UE_MAJOR.$UE_MINOR.$UE_PATCH" | tee -a "$LOG_FILE"
uname -a | tee -a "$LOG_FILE"

run_step() {
  local label="$1"
  shift
  echo "" | tee -a "$LOG_FILE"
  echo ">>> $label" | tee -a "$LOG_FILE"
  set +e
  "$@" 2>&1 | tee -a "$LOG_FILE"
  local status=${PIPESTATUS[0]}
  set -e
  if [[ "$status" -ne 0 ]]; then
    echo "FAILED: $label (exit $status)" | tee -a "$LOG_FILE"
    exit "$status"
  fi
}

echo "" | tee -a "$LOG_FILE"
echo ">>> PHASE 1/4 — UnrealHeaderTool" | tee -a "$LOG_FILE"
run_step "UHT preflight for OmnisEditor" "$RUN_UBT" -Mode=UnrealHeaderTool "-Target=OmnisEditor Linux Development" "-Project=$PROJECT_FILE"

if ! find "$ROOT/Intermediate/Build" -type f \( -name '*.generated.h' -o -name '*.gen.cpp' \) -print -quit 2>/dev/null | grep -q .; then
  echo "UHT did not leave generated reflection output under Intermediate/Build." | tee -a "$LOG_FILE" >&2
  exit 1
fi

echo "" | tee -a "$LOG_FILE"
echo ">>> PHASE 2/4 — UBT + compile + link Editor target" | tee -a "$LOG_FILE"
run_step "UBT OmnisEditor Linux Development" "$RUN_UBT" OmnisEditor Linux Development "-Project=$PROJECT_FILE" -WaitMutex

echo "" | tee -a "$LOG_FILE"
echo ">>> PHASE 3/4 — UBT + compile + link Game target" | tee -a "$LOG_FILE"
run_step "UBT Omnis Linux Development" "$RUN_UBT" Omnis Linux Development "-Project=$PROJECT_FILE" -WaitMutex

echo "" | tee -a "$LOG_FILE"
echo ">>> PHASE 4/4 — output verification" | tee -a "$LOG_FILE"
if [[ ! -d "$ROOT/Binaries" ]]; then
  echo "Project Binaries directory was not generated." | tee -a "$LOG_FILE" >&2
  exit 1
fi

if ! find "$ROOT/Binaries" -type f -iname '*Omnis*' -print -quit 2>/dev/null | grep -q .; then
  echo "No Omnis binary/module artifact was found under Binaries after successful UBT builds." | tee -a "$LOG_FILE" >&2
  exit 1
fi

echo "=== RESULT: UE 5.6 UHT + UBT + module link + Editor/Game build PASSED ===" | tee -a "$LOG_FILE"
