#!/usr/bin/env bash
set -euo pipefail
set +x

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

command -v npx >/dev/null 2>&1 || { echo "npx is required." >&2; exit 1; }
command -v openssl >/dev/null 2>&1 || { echo "openssl is required." >&2; exit 1; }

echo "Preparing protected GitLab OAuth operator credentials for Worker: omnis-game"
echo "OAuth stays disabled in wrangler.jsonc until an explicit activation change is reviewed."

read -r -p "Allowed numeric GitLab user ID(s), comma-separated: " ALLOWED_USER_IDS
ALLOWED_USER_IDS="$(printf "%s" "$ALLOWED_USER_IDS" | tr -d '[:space:]')"
if [[ ! "$ALLOWED_USER_IDS" =~ ^[0-9]+(,[0-9]+)*$ ]]; then
  echo "Provide one or more numeric GitLab user IDs separated by commas." >&2
  exit 1
fi

read -r -s -p "GitLab Omnis Runner Control Client Secret: " GITLAB_OAUTH_CLIENT_SECRET
printf "\n"
if [[ -z "$GITLAB_OAUTH_CLIENT_SECRET" ]]; then
  echo "Client secret cannot be empty." >&2
  exit 1
fi

CONFIG_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/omnis"
DELIVERY_SECRET_FILE="$CONFIG_DIR/runner-delivery.secret"
install -d -m 700 "$CONFIG_DIR"
if [[ -L "$DELIVERY_SECRET_FILE" ]]; then
  echo "Refusing a symbolic-link delivery-secret file: $DELIVERY_SECRET_FILE" >&2
  exit 1
elif [[ -e "$DELIVERY_SECRET_FILE" ]]; then
  [[ -f "$DELIVERY_SECRET_FILE" && -s "$DELIVERY_SECRET_FILE" ]] || {
    echo "Existing delivery-secret file is not a non-empty regular file." >&2
    exit 1
  }
  chmod 600 "$DELIVERY_SECRET_FILE"
  OMNIS_RUNNER_DELIVERY_SECRET="$(cat "$DELIVERY_SECRET_FILE")"
else
  OMNIS_RUNNER_DELIVERY_SECRET="$(openssl rand -base64 48 | tr -d '\n')"
  (umask 077; printf "%s" "$OMNIS_RUNNER_DELIVERY_SECRET" > "$DELIVERY_SECRET_FILE")
  chmod 600 "$DELIVERY_SECRET_FILE"
fi

trap 'unset GITLAB_OAUTH_CLIENT_SECRET ALLOWED_USER_IDS OMNIS_RUNNER_DELIVERY_SECRET' EXIT

printf "%s" "$GITLAB_OAUTH_CLIENT_SECRET" |
  npx wrangler secret put GITLAB_OAUTH_CLIENT_SECRET --config wrangler.jsonc
printf "%s" "$ALLOWED_USER_IDS" |
  npx wrangler secret put GITLAB_OAUTH_ALLOWED_USER_IDS --config wrangler.jsonc
printf "%s" "$OMNIS_RUNNER_DELIVERY_SECRET" |
  npx wrangler secret put OMNIS_RUNNER_DELIVERY_SECRET --config wrangler.jsonc

echo "OAuth operator secrets uploaded."
echo "Existing OMNIS_OAUTH_ENCRYPTION_KEY was not rotated or overwritten."
echo "A protected delivery-secret copy was saved outside the repository."
echo "GITLAB_OAUTH_ENABLED remains false. Review allowlist, callback and runner policy before enabling and deploying."
