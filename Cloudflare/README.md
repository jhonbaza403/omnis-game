# Cloudflare integration

Omnis uses a real Cloudflare Worker with Queues, a dead-letter queue, and Workers Observability.

## Worker
- Name: `omnis-game`
- Entry point: `Cloudflare/worker.js`
- Wrangler configuration: `/wrangler.json`
- `workers.dev`: enabled for initial access; use a custom domain for business-critical production traffic.

## Queues
- Producer binding: `OMNIS_EVENTS` -> `omnis-events`
- Consumer: `omnis-game`
- Batch size: 10
- Timeout: 5 seconds
- Max retries: 3
- Retry delay: 10 seconds
- Max concurrency: 5
- Dead-letter queue: `omnis-events-dlq`

## Observability
- Workers Observability enabled.
- Invocation logs enabled and persisted.
- Logs sampled at 100%.
- Query-string redaction configured in Wrangler.
- Real-time Issues enabled.
- Traces enabled and sampled at 10%.

## Domain
No custom zone/domain is currently present in the Cloudflare account, so no custom-domain route is configured. The Worker is exposed through `workers.dev` until a domain is added.

## Cloudflare Builds
Cloudflare Workers Builds supports GitLab repositories and automatic deployment on push. The account currently reports that its GitLab account connection is disconnected, so repository auto-build cannot be safely activated by API until that GitLab connection is re-authorized in Cloudflare.

GitLab has a `cloudflare_validate` job that performs a real Wrangler dry-run without storing provider secrets in the repository.
