#!/usr/bin/env sh
set -eu

PROJECT_PATH="${UNITY_PROJECT_PATH:-UnityProject}"

test -f "$PROJECT_PATH/ProjectSettings/ProjectVersion.txt"
grep -q '^m_EditorVersion: ' "$PROJECT_PATH/ProjectSettings/ProjectVersion.txt"
test -f "$PROJECT_PATH/Packages/manifest.json"
test -d "$PROJECT_PATH/Assets"

if [ ! -f "$PROJECT_PATH/Assets/Scenes/OmnisMain.unity" ]; then
  echo "Unity scaffold is valid; gameplay migration is incomplete (main scene absent)."
else
  echo "Unity main scene exists. The build job can be enabled after license setup."
fi
