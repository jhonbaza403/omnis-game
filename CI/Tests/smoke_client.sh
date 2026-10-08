#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$BASH_SOURCE")/../.." && pwd)"
LOG_FILE="$ROOT/Saved/Logs/omnis_client_smoke.log"
CLIENT_BIN="$(find "$ROOT/Build/Omnis" -maxdepth 6 -type f -perm -u+x -iname 'Omnis*' -print -quit 2>/dev/null || true)"
[[ -n "$CLIENT_BIN" ]] || { echo "Packaged Omnis client executable not found." >&2; exit 1; }
mkdir -p "$(dirname "$LOG_FILE")"
PORT="${OMNIS_CLIENT_SMOKE_PORT:-17778}"
timeout 45s "$CLIENT_BIN" "/Game/Omnis/Maps/Omnis_OpenWorld" -game -nullrhi -unattended -log "-port=$PORT" >"$LOG_FILE" 2>&1 &
PID=$!
cleanup(){ kill "$PID" >/dev/null 2>&1 || true; wait "$PID" >/dev/null 2>&1 || true; }
trap cleanup EXIT
for _ in {1..45}; do
  if grep -Eq 'LogInit: Display: Running engine for game|Browse.*Omnis_OpenWorld|OmnisGameMode|World Partition' "$LOG_FILE"; then
    echo "=== RESULT: PACKAGED OMNIS CLIENT BOOTSTRAP PASSED ==="
    tail -n 120 "$LOG_FILE"
    exit 0
  fi
  if ! kill -0 "$PID" >/dev/null 2>&1; then
    echo "Omnis client exited before reaching runtime bootstrap." >&2
    cat "$LOG_FILE" >&2
    exit 1
  fi
  sleep 1
done
echo "Omnis client did not reach runtime bootstrap within 45 seconds." >&2
cat "$LOG_FILE" >&2
exit 1
