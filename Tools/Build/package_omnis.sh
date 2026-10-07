#!/usr/bin/env bash
set -eo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
UE_ENGINE_ROOT="$(printenv UE_ENGINE_ROOT || true)"

if [[ -z "$UE_ENGINE_ROOT" ]]; then
  echo "UE_ENGINE_ROOT must point to Unreal Engine 5.8 on the dedicated runner." >&2
  exit 1
fi

UAT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
if [[ ! -x "$UAT" ]]; then
  echo "RunUAT.sh not found or not executable: $UAT" >&2
  exit 1
fi

if [[ ! -f "$PROJECT_FILE" ]]; then
  echo "Omnis.uproject not found: $PROJECT_FILE" >&2
  exit 1
fi

PLATFORM="$(printenv UE_TARGET_PLATFORM || true)"
CONFIGURATION="$(printenv UE_BUILD_CONFIGURATION || true)"
ARCHIVE_DIR="$(printenv UE_ARCHIVE_DIR || true)"

if [[ -z "$PLATFORM" ]]; then PLATFORM="Linux"; fi
if [[ -z "$CONFIGURATION" ]]; then CONFIGURATION="Development"; fi
if [[ -z "$ARCHIVE_DIR" ]]; then ARCHIVE_DIR="$ROOT/Build/Omnis/$PLATFORM"; fi

mkdir -p "$ARCHIVE_DIR"

"$UAT" BuildCookRun   -project="$PROJECT_FILE"   -noP4   -build   -cook   -stage   -package   -archive   -archivedirectory="$ARCHIVE_DIR"   -pak   -targetplatform="$PLATFORM"   -clientconfig="$CONFIGURATION"   -utf8output

echo "=== Omnis package created: $ARCHIVE_DIR ==="
