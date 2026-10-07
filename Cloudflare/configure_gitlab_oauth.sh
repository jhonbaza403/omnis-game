#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

command -v npx >/dev/null 2>&1 || { echo "npx is required." >&2; exit 1; }
command -v openssl >/dev/null 2>&1 || { echo "openssl is required to generate the encryption key." >&2; exit 1; }

echo "Configuring production GitLab OAuth secrets for Worker: omnis-game"
echo "The GitLab client secret is read silently and is never printed."

read -r -s -p "Paste the GitLab Omnis Runner Control Client Secret: " GITLAB_OAUTH_CLIENT_SECRET
printf "\n"
if [[ -z "$GITLAB_OAUTH_CLIENT_SECRET" ]]; then
  echo "Client secret cannot be empty." >&2
  exit 1
fi

OMNIS_OAUTH_ENCRYPTION_KEY="$(openssl rand -base64 32)"
printf "%s" "$GITLAB_OAUTH_CLIENT_SECRET" | npx wrangler secret put GITLAB_OAUTH_CLIENT_SECRET --config wrangler.jsonc
printf "%s" "$OMNIS_OAUTH_ENCRYPTION_KEY" | npx wrangler secret put OMNIS_OAUTH_ENCRYPTION_KEY --config wrangler.jsonc

echo "Production OAuth secrets configured."
echo "OAuth remains disabled in Worker Previews by design; no preview client secret is required."
echo "Deploy production with: npm run cf:deploy"
