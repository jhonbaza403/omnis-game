# Cloudflare — Omnis: El Último Ciclo

La integración de Omnis usa Cloudflare Workers con Wrangler administrado desde el propio proyecto.

## Fuente de verdad

- Worker: `omnis-game`
- Entry point: `Cloudflare/worker.js`
- Configuración: `wrangler.jsonc`
- Wrangler: `4.148.0` fijado en `package.json`
- Node: `24+`
- `workers.dev`: habilitado
- Preview URLs: habilitadas y activadas en Cloudflare (`previews_enabled=true`)

Cloudflare recomienda `wrangler.jsonc` para proyectos nuevos y Wrangler 4.135.0 o posterior para Worker Previews. Omnis usa 4.148.0, fijado explícitamente en el proyecto.

## Worker Previews

Producción usa:

`npm run cf:deploy`

Las ramas usan:

`npm run cf:preview`

Cada Preview usa la configuración de `previews`, incluyendo:

- `OMNIS_ENVIRONMENT=preview`
- observabilidad propia
- logs persistidos
- trazas persistidas al 100%
- Issues habilitados

Las Previews no reutilizan consumidores de Queue de producción. El consumidor `omnis-events` permanece únicamente en la configuración superior de producción, conforme al modelo actual de aislamiento de Previews.

## Observabilidad

Producción y cada Preview tienen logs, trazas e Issues habilitados desde Wrangler. La telemetría de Preview queda separada de producción.

## Secretos

Los secretos de Preview no se escriben en Git. Wrangler permite mantener secretos en la configuración base de Previews o en una Preview concreta:

`npx wrangler preview base-config secret put <KEY>`

`npx wrangler preview secret put <KEY> --name <preview-name>`

Los secretos no definidos en la configuración quedan fuera del repositorio.

## GitLab CI

El job `cloudflare_validate` instala la dependencia local exacta y ejecuta:

`npm run cf:check`

El job `cloudflare_preview` ejecuta `npx wrangler preview` para ramas y merge requests cuando `CLOUDFLARE_ACCOUNT_ID` y `CLOUDFLARE_API_TOKEN` están disponibles como variables CI. El nombre del Preview deriva de `CI_COMMIT_REF_SLUG`.

## Workers Builds

El Worker `omnis-game` existe en Cloudflare y sus Preview URLs de `workers.dev` están activadas. La conexión GitLab de Workers Builds sigue desconectada en la cuenta. La API de Cloudflare rechaza la vinculación porque falta la autorización de la aplicación GitLab.

Cuando la integración GitLab se reautorice en Cloudflare, Workers Builds podrá crear las Previews automáticamente y publicar sus URLs en los merge requests. El comando de Preview recomendado es `npx wrangler preview`, usando la versión declarada por el `package.json`.

No se usan Version URLs ni `wrangler versions upload --preview-alias` para probar ramas; el flujo actual recomendado para este caso es Worker Previews.

## Política

No se crean UAsset/UMAP falsos, bindings ficticios ni servicios externos simulados. La materialización Unreal continúa dependiendo del runner UE 5.6 real.
