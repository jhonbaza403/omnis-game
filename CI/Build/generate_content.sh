#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
PROJECT_FILE="$ROOT/Omnis.uproject"
BASE_SCRIPT="$ROOT/Tools/Editor/GenerateOmnisWorldContent.py"
PREMIUM_SCRIPT="$ROOT/Tools/Editor/MaterializeOmnisPremiumWorld.py"
: "${UE_ENGINE_ROOT:?UE_ENGINE_ROOT must point to Unreal Engine 5.6}"
EDITOR="$UE_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd"
LOG_FILE="$ROOT/Saved/Logs/omnis_content_generation.log"
MAP_FILE="$ROOT/Content/Omnis/Maps/Omnis_OpenWorld.umap"
ART_ROOT="$ROOT/Content/Omnis/Art"
MAT_ROOT="$ROOT/Content/Omnis/Materials"
[[ -x "$EDITOR" ]] || { echo "UnrealEditor-Cmd not found: $EDITOR" >&2; exit 1; }
[[ -f "$PROJECT_FILE" && -f "$BASE_SCRIPT" && -f "$PREMIUM_SCRIPT" ]] || { echo "Project or world materializers missing." >&2; exit 1; }
mkdir -p "$(dirname "$LOG_FILE")"; rm -f "$MAP_FILE"
"$EDITOR" "$PROJECT_FILE" -unattended -nop4 -nosplash -nullrhi -ExecutePythonScript="$PREMIUM_SCRIPT" 2>&1 | tee "$LOG_FILE"
[[ -s "$MAP_FILE" ]] || { echo "Native .umap was not generated." >&2; exit 1; }
NATIVE="$(find "$ART_ROOT" -type f -name '*.uasset' | wc -l | tr -d ' ')"
MATS="$(find "$MAT_ROOT" -type f -name '*.uasset' | wc -l | tr -d ' ')"
[[ "$NATIVE" -ge 63 ]] || { echo "Expected >=63 native assets; got $NATIVE." >&2; exit 1; }
[[ "$MATS" -ge 10 ]] || { echo "Expected >=10 material assets; got $MATS." >&2; exit 1; }
grep -q "REAL PREMIUM WORLD MATERIALIZED" "$LOG_FILE" || { echo "Generator did not certify materialization." >&2; exit 1; }
echo "=== REAL UE5.6 PREMIUM CONTENT GENERATION PASSED ==="
echo "Map: $MAP_FILE"; echo "Native Art assets: $NATIVE"; echo "Material assets: $MATS"
