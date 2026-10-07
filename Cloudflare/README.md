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

No se crean UAsset/UMAP falsos, bindings ficticios ni servicios externos simulados. La materialización Unreal continúa dependiendo del runner UE 5.8 real.

## GitLab OAuth — Omnis Runner Control

El Worker implementa un flujo OAuth 2.0 de servidor para la aplicación **Omnis Runner Control**:

- Inicio: `GET /oauth/start`
- Callback: `GET /oauth/callback`
- Estado: `GET /oauth/status`
- Scopes mínimos: `create_runner manage_runner`
- Callback registrado: `https://omnis-game.bazajhon.workers.dev/oauth/callback`

El estado OAuth es de un solo uso y expira en 10 minutos. El token de GitLab se cifra con AES-256-GCM antes de persistirse en Workers KV. El Worker nunca devuelve el access token ni el refresh token en una respuesta.

GitLab devuelve por defecto OAuth access tokens con una vida de dos horas y refresh tokens que permiten obtener credenciales nuevas; el flujo del Worker actualiza y reemplaza de forma automática la credencial cifrada cuando se aproxima la expiración. Esto sigue el comportamiento documentado por GitLab. 

### Secretos requeridos

No se almacenan en Git:

- `GITLAB_OAUTH_CLIENT_SECRET` — secreto de la aplicación GitLab **Omnis Runner Control**.
- `OMNIS_OAUTH_ENCRYPTION_KEY` — secreto de 32 bytes codificado en Base64 para cifrar los tokens.

Para producción:

`npx wrangler secret put GITLAB_OAUTH_CLIENT_SECRET`

`npx wrangler secret put OMNIS_OAUTH_ENCRYPTION_KEY`

Para Preview deben configurarse también como secretos de Preview/Base según el flujo actual de Wrangler. Nunca se deben añadir al repositorio, a `wrangler.jsonc` ni a logs.

El Client Secret de GitLab solo estuvo disponible una vez al crear la aplicación; si se perdió o fue expuesto, usa **Renew secret** en GitLab y registra el nuevo secreto únicamente en Cloudflare. 

### Estado de la integración OAuth

El código del callback ya está desplegado en el Worker `omnis-game` y el Worker tiene el binding de producción `OMNIS_OAUTH_KV`. El listado de secretos de Cloudflare no muestra todavía credenciales configuradas; por diseño, el Client Secret nunca se almacena en Git ni se devuelve por API.

El último paso de activación es ejecutar `bash Cloudflare/configure_gitlab_oauth.sh` en una máquina confiable con acceso a Wrangler. El script solicita el Client Secret de forma silenciosa, genera una clave de cifrado nueva y guarda ambos valores únicamente como secretos de producción.

El formulario OAuth de GitLab debe tener exactamente este callback:
`https://omnis-game.bazajhon.workers.dev/oauth/callback`

No se utiliza `localhost` en producción.
