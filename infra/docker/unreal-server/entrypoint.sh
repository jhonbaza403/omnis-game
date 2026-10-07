#!/usr/bin/env bash
set -euo pipefail

ROOT=/opt/omnis/server
SERVER_BIN="${OMNIS_SERVER_EXECUTABLE:-}"
if [[ -z "$SERVER_BIN" ]]; then
  SERVER_BIN="$(find "$ROOT" -maxdepth 4 -type f -perm -u+x -iname 'OmnisServer*' -print -quit)"
fi
if [[ -z "$SERVER_BIN" || ! -x "$SERVER_BIN" ]]; then
  echo "No real OmnisServer executable found in $ROOT. Build/publish the UE 5.6 dedicated-server package first." >&2
  exit 1
fi

exec "$SERVER_BIN"   "${OMNIS_SERVER_MAP:-/Game/Omnis/Maps/Omnis_OpenWorld}?listen"   -server   -log   "-port=${OMNIS_SERVER_PORT:-7777}"
