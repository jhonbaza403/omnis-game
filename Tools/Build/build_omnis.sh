#!/usr/bin/env bash
set -euo pipefail

: "${UE_ENGINE_ROOT:?Set UE_ENGINE_ROOT to the Unreal Engine installation}"
ROOT="$(cd "$(dirname "$BASH_SOURCE[0]")/../.." && pwd)"
UAT="$UE_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"

if [[ ! -x "$UAT" ]]; then
  echo "RunUAT.sh not found at $UAT" >&2
  exit 1
fi

"$UAT" BuildCookRun -project="$ROOT/Omnis.uproject" -noP4 -build -stage -prereqs -pak -archive -archivedirectory="$ROOT/Saved/Build"
