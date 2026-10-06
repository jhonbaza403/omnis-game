# Cloudflare Worker

This directory contains the real Cloudflare Worker integration for Omnis.

The Unreal Engine project is not a static web application, so Wrangler is configured with an explicit Worker entry point. The authoritative Wrangler configuration is the root-level wrangler.json.

## Cloudflare build settings

- Root directory: /
- Build command: none
- Deploy command: npx wrangler deploy

Wrangler reads /wrangler.json, whose main points to Cloudflare/worker.js.

The Worker exposes:

- GET / — service metadata
- GET /health — health status
- Other paths — HTTP 404

This is a real Worker deployment; it does not fabricate static build output or package the Unreal project as a web application.
