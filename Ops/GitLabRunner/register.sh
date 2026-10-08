#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$BASH_SOURCE")"

: "${RUNNER_AUTHENTICATION_TOKEN:?Set RUNNER_AUTHENTICATION_TOKEN to your GitLab project runner token (glrt-...)}"

case "$RUNNER_AUTHENTICATION_TOKEN" in
  glrt-*) ;;
  *) echo "Runner authentication token must begin with glrt-" >&2; exit 1 ;;
esac

mkdir -p config cache

docker compose run --rm gitlab-runner register \
  --non-interactive \
  --url "https://gitlab.com/" \
  --token "$RUNNER_AUTHENTICATION_TOKEN" \
  --executor "docker" \
  --docker-image "ubuntu:24.04" \
  --docker-pull-policy "if-not-present" \
  --description "omnis-ue58-linux" \
  --tag-list "omnis-ci-linux" \
  --run-untagged="false" \
  --locked="true" \
  --access-level="not_protected" \
  --docker-privileged="true" \
  --docker-volumes "/cache" \
  --docker-volumes "/var/run/docker.sock:/var/run/docker.sock" \
  --docker-shm-size "17179869184" \
  --docker-pull-filter "always"

echo "Runner registration complete."
