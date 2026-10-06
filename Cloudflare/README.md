# Cloudflare Worker

This directory contains the real Cloudflare Worker integration for Omnis.

The Unreal Engine project is not a web application and therefore does not deploy through a static-site directory such as `dist/` or `public/`. Wrangler is configured with an explicit Worker entry point instead.

## Cloudflare build settings

- Root directory: `/`
- Build command: none
- Deploy command: `npx wrangler deploy --config Cloudflare/wrangler.json`

The Worker exposes:

- `GET /` — service metadata
- `GET /health` — health status
- Other paths — HTTP 404

This is a real Worker deployment; it does not fabricate static build output or package the Unreal project as a web application.
