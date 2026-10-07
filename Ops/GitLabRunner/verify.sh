#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$BASH_SOURCE")"
docker compose up -d
docker compose exec gitlab-runner gitlab-runner verify --delete
docker compose exec gitlab-runner gitlab-runner list
