#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RUNNER_DIR="$ROOT/Ops/GitLabRunner"

[[ "$(uname -m)" == "x86_64" ]] || { echo "Omnis UE 5.8.3 runner requires x86_64/amd64." >&2; exit 1; }
command -v docker >/dev/null 2>&1 || { echo "Docker Engine is required." >&2; exit 1; }
docker info >/dev/null 2>&1 || { echo "Docker Engine is not reachable by the current user." >&2; exit 1; }
docker compose version >/dev/null 2>&1 || { echo "Docker Compose v2 is required." >&2; exit 1; }

CPU="$(nproc)"
MEM_KB="$(awk "/MemTotal/ {print \$2}" /proc/meminfo)"
DISK_KB="$(df -Pk "$ROOT" | awk "NR==2 {print \$4}")"

echo "CPU=$CPU"
echo "MEM_KB=$MEM_KB"
echo "FREE_DISK_KB=$DISK_KB"
test "$CPU" -ge 8 || { echo "Need at least 8 CPU." >&2; exit 1; }
test "$MEM_KB" -ge 24000000 || { echo "Need at least 24 GB RAM." >&2; exit 1; }
test "$DISK_KB" -ge 80000000 || { echo "Need at least 80 GB free disk." >&2; exit 1; }

: "${RUNNER_AUTHENTICATION_TOKEN:?Set RUNNER_AUTHENTICATION_TOKEN to the glrt- token shown by Omnis Runner Control}"
case "$RUNNER_AUTHENTICATION_TOKEN" in glrt-*) ;; *) echo "Runner token must begin with glrt-." >&2; exit 1 ;; esac

cd "$RUNNER_DIR"
docker compose pull
bash ./register.sh
docker compose up -d
docker compose exec gitlab-runner gitlab-runner verify --delete
docker compose exec gitlab-runner gitlab-runner list

echo "=== OMNIS UE 5.8.3 RUNNER HOST READY ==="
