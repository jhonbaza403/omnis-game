#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
SERVER_BIN="$(find "$ROOT/Build/OmnisServer" -maxdepth 5 -type f -perm -u+x -iname 'OmnisServer*' -print -quit 2>/dev/null || true)"
LOG_FILE="$ROOT/Saved/Logs/omnis_dedicated_server_smoke.log"

[[ -n "$SERVER_BIN" ]] || { echo "Packaged OmnisServer executable not found." >&2; exit 1; }
mkdir -p "$(dirname "$LOG_FILE")"

PORT="${OMNIS_SMOKE_PORT:-17777}"
timeout 30s "$SERVER_BIN"   "/Game/Omnis/Maps/Omnis_OpenWorld?listen"   -server   -log   -unattended   -nullrhi   "-port=$PORT" >"$LOG_FILE" 2>&1 &
PID=$!

cleanup() {
  kill "$PID" >/dev/null 2>&1 || true
  wait "$PID" >/dev/null 2>&1 || true
}
trap cleanup EXIT

for _ in {1..30}; do
  if ! kill -0 "$PID" >/dev/null 2>&1; then
    echo "OmnisServer exited before smoke-test readiness." >&2
    cat "$LOG_FILE" >&2
    exit 1
  fi

  if grep -Eq 'GameNetDriver|LogNetVersion|Browse.*Omnis_OpenWorld|NetDriver' "$LOG_FILE"; then
    echo "=== RESULT: OmnisServer process and network driver startup detected ==="
    tail -n 80 "$LOG_FILE"
    exit 0
  fi

  sleep 1
done

echo "OmnisServer did not reach network startup within 30 seconds." >&2
cat "$LOG_FILE" >&2
exit 1
