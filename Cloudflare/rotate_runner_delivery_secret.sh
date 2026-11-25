#!/usr/bin/env bash
set -euo pipefail
set +x

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
command -v npx >/dev/null 2>&1 || { echo "npx is required." >&2; exit 1; }
command -v openssl >/dev/null 2>&1 || { echo "openssl is required." >&2; exit 1; }

CONFIG_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/omnis"
DELIVERY_SECRET_FILE="$CONFIG_DIR/runner-delivery.secret"
install -d -m 700 "$CONFIG_DIR"
if [[ -L "$DELIVERY_SECRET_FILE" ]]; then
  echo "Refusing a symbolic-link delivery-secret file." >&2
  exit 1
fi
TEMP_SECRET_FILE="$(mktemp "$CONFIG_DIR/runner-delivery.secret.XXXXXX")"
chmod 600 "$TEMP_SECRET_FILE"
cleanup() { rm -f "$TEMP_SECRET_FILE"; unset NEW_DELIVERY_SECRET; }
trap cleanup EXIT
NEW_DELIVERY_SECRET="$(openssl rand -base64 48 | tr -d '\n')"
printf "%s" "$NEW_DELIVERY_SECRET" > "$TEMP_SECRET_FILE"
printf "%s" "$NEW_DELIVERY_SECRET" | npx wrangler secret put OMNIS_RUNNER_DELIVERY_SECRET --config wrangler.jsonc
mv -f "$TEMP_SECRET_FILE" "$DELIVERY_SECRET_FILE"
chmod 600 "$DELIVERY_SECRET_FILE"
echo "Runner delivery bearer rotated in Cloudflare and the protected local copy was replaced."
echo "This rotates only the M2M delivery bearer; it does not revoke an issued GitLab runner token."
