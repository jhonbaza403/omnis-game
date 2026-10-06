# Cloudflare integration

Omnis uses a real Cloudflare Worker with Queues and Workers Observability.

## Worker

- Name: `omnis-game`
- Entry point: `Cloudflare/worker.js`
- Wrangler configuration: `/wrangler.json`

## Queue

- Primary queue: `omnis-events`
- Dead-letter queue: `omnis-events-dlq`
- Consumer: Worker `omnis-game`
- The Worker implements a `queue(batch)` handler and writes structured JSON logs for each delivered message.

## Observability

Worker Observability is enabled in `wrangler.json`: invocation logs, persisted logs, Real-time Issues, and traces. Query strings are redacted in logs/traces.

## Domain

No custom DNS zone is currently present in the Cloudflare account. Production custom-domain routing must be attached only after the intended domain is added to the account; until then, the Worker can use `workers.dev` for non-critical access.

## Workers Builds

Cloudflare Workers Builds should point to this GitLab repository and branch `main`, with `/` as the root directory and `npx wrangler deploy` as the deploy command.

