#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"

: "${UE_ENGINE_ROOT:?Set UE_ENGINE_ROOT to the Unreal Engine 5.6 installation}"
: "${OMNIS_PLATFORM:?Set OMNIS_PLATFORM to Win64, PS5, XSX, or Switch}"

case "$OMNIS_PLATFORM" in
  Win64)
    UAT_PLATFORM="Win64"
    ;;
  PS5)
    UAT_PLATFORM="PS5"
    : "${OMNIS_CONSOLE_SDK_READY:?Set OMNIS_CONSOLE_SDK_READY=true only on an authorized PlayStation runner}"
    [[ "$OMNIS_CONSOLE_SDK_READY" == "true" ]]
    ;;
  XSX)
    UAT_PLATFORM="XSX"
    : "${OMNIS_CONSOLE_SDK_READY:?Set OMNIS_CONSOLE_SDK_READY=true only on an authorized Xbox runner}"
    [[ "$OMNIS_CONSOLE_SDK_READY" == "true" ]]
    ;;
  Switch)
    UAT_PLATFORM="Switch"
    : "${OMNIS_CONSOLE_SDK_READY:?Set OMNIS_CONSOLE_SDK_READY=true only on an authorized Nintendo runner}"
    [[ "$OMNIS_CONSOLE_SDK_READY" == "true" ]]
    ;;
  *)
    echo "Unsupported OMNIS_PLATFORM: $OMNIS_PLATFORM" >&2
    exit 2
    ;;
esac

RUN_UAT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
if [[ ! -x "$RUN_UAT" ]]; then
  echo "RunUAT.sh not found: $RUN_UAT" >&2
  exit 1
fi

if [[ ! -f "$PROJECT_FILE" ]]; then
  echo "Omnis.uproject not found: $PROJECT_FILE" >&2
  exit 1
fi

VERSION_FILE="$UE_ENGINE_ROOT/Engine/Build/Build.version"
if [[ ! -f "$VERSION_FILE" ]]; then
  echo "Unreal Engine Build.version not found: $VERSION_FILE" >&2
  exit 1
fi

UE_MAJOR="$(sed -n 's/.*"MajorVersion"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "$VERSION_FILE" | head -n 1)"
UE_MINOR="$(sed -n 's/.*"MinorVersion"[[:space:]]*:[[:space:]]*\([0-9][0-9]*\).*/\1/p' "$VERSION_FILE" | head -n 1)"
if [[ "$UE_MAJOR.$UE_MINOR" != "5.6" ]]; then
  echo "Wrong Unreal Engine version: $UE_MAJOR.$UE_MINOR (required 5.6.x)" >&2
  exit 1
fi

ARCHIVE_DIR="$ROOT/Saved/Archived/$OMNIS_PLATFORM/${CI_COMMIT_SHA:-local}"
mkdir -p "$ARCHIVE_DIR"

"$RUN_UAT" BuildCookRun \
  "-project=$PROJECT_FILE" \
  "-noP4" \
  "-build" \
  "-cook" \
  "-stage" \
  "-pak" \
  "-archive" \
  "-archivedirectory=$ARCHIVE_DIR" \
  "-platform=$UAT_PLATFORM" \
  "-clientconfig=Shipping" \
  "-utf8output"
