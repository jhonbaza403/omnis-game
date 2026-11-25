# Cloudflare — Omnis: El Último Ciclo

## Papel operativo y fuente de verdad

Cloudflare Worker es una herramienta auxiliar de administración y observabilidad. No es el motor del juego, no es la fuente de código ni el pipeline oficial de compilación/publicación.

- Repositorio fuente: GitHub `jhonbaza403/omnis-game`.
- Motor único de release: Unreal Engine 5.8.
- Proyecto secundario conservado: Unity 6.3 en `UnityProject/`.
- CI oficial de release: GitHub Actions; la certificación nativa UE 5.8 es una puerta separada del CI estático.
- GitLab CI: validaciones auxiliares, Worker y builds opcionales del proyecto Unity.
- Cloudflare Worker: servicio operativo auxiliar; su despliegue no publica ni certifica Omnis.

## Estado real comprobado — 2026-11-03

Se consultó la configuración del Worker en la cuenta Cloudflare. Se leyeron nombres y tipos de bindings/secretos, nunca valores secretos.

- El script `omnis-game` existe.
- La última implementación registrada por la API era del 9 de octubre de 2026 a las 02:10 UTC; el código de seguridad actualizado en esta rama todavía no está desplegado.
- El único secreto activo enumerado fue `OMNIS_OAUTH_ENCRYPTION_KEY`.
- No aparecen `GITLAB_OAUTH_CLIENT_SECRET`, `GITLAB_OAUTH_ALLOWED_USER_IDS` ni `OMNIS_RUNNER_DELIVERY_SECRET`.
- La variable activa `GITLAB_OAUTH_ENABLED` tampoco aparece en la configuración del Worker desplegado.
- Los bindings `OMNIS_OAUTH_KV` y `OMNIS_EVENTS` sí están asociados.

**Conclusión:** OAuth de producción no está acreditado como activado ni configurado. La presencia de una variable en `wrangler.jsonc` no prueba que exista en el Worker desplegado. No afirmar conexión, provisión ni funcionamiento hasta verificarlo contra el Worker activo.

## Previews y despliegues

El repositorio deja OAuth deshabilitado (`GITLAB_OAUTH_ENABLED=false`) tanto en producción declarativa como en Previews. Las Previews no comparten consumidor de Queue de producción.

- `npm run cf:check`: validación/dry-run.
- `npm run cf:preview`: comprobación de Preview.
- `npm run cf:deploy`: despliegue explícito del Worker auxiliar.

El job GitLab `cloudflare_deploy_production` es manual y pertenece a la operación del Worker; no es una aprobación de release del videojuego.

## Runner Control: autorización y sesiones

El Worker corregido aplica estas reglas:

1. OAuth requiere `GITLAB_OAUTH_ALLOWED_USER_IDS`, una allowlist de IDs numéricos de operadores.
2. El callback comprueba el ID real devuelto por GitLab `GET /user` antes de guardar la credencial y crear una sesión.
3. La cookie contiene un ID opaco y aleatorio; la sesión se valida en KV y expira a los 30 minutos.
4. La cookie de sesión es `HttpOnly`, `Secure` y `SameSite=Strict`; el estado OAuth es de un solo uso y expira a los 10 minutos.
5. `/runner`, `/runner/provision` y `/runner/status` exigen sesión válida de una identidad incluida en la allowlist.
6. Los POST de navegador requieren Origin del mismo origen y, cuando existe, `Sec-Fetch-Site: same-origin`.
7. `POST /runner/logout` elimina la sesión actual de KV y la cookie.
8. Origin es defensa CSRF complementaria; no sustituye sesión, identidad ni allowlist.

El aprovisionamiento registra `requested_by_gitlab_user_id` y crea un runner limitado a referencias protegidas (`access_level=ref_protected`). Las ramas/tags de GitLab que deban usarlo también tienen que estar protegidos.

## Entrega del token del runner

`POST /runner/provision` devuelve metadatos, nunca el token. El token de autenticación del runner se cifra con AES-256-GCM antes de guardarse en KV y expira a los 15 minutos.

La entrega M2M también queda deshabilitada cuando `GITLAB_OAUTH_ENABLED=false`, lo que permite un interruptor de parada común. El endpoint separado `POST /runner/registration-token` requiere `Authorization: Bearer <OMNIS_RUNNER_DELIVERY_SECRET>` y rechaza solicitudes con Origin o cookie de navegador. Así, el navegador nunca recibe el token.

La ruta M2M devuelve la credencial al cliente de terminal autorizado porque el registro del runner la necesita. La entrega **no es atómicamente de un solo uso**: puede repetirse mientras el TTL sea válido. La herramienta cliente debe guardar la respuesta solo en un almacén protegido; no imprimirla, no pasarla a `echo`, no incluirla en logs, capturas, tickets ni historial de shell.

El bearer de entrega es sensible: mantenerlo fuera de Git, usar permisos locales restrictivos y rotarlo inmediatamente si se expone. El token de runner guardado expira en 15 minutos; si la provisión fue creada y no se entregó, no crear otro runner automáticamente. Revisar/revocar el runner existente desde GitLab.

El Worker crea el runner con tag `omnis-ci-linux`; el `.gitlab-ci.yml` actual usa `omnis-linux-docker` como tag general. No hay evidencia de que el runner creado sea consumido por los jobs existentes. Alinear tag, executor, herramienta UE 5.8 y job consumidor deliberadamente antes de declararlo utilizable.

## Secretos y activación

No se guardan secretos en Git ni en `wrangler.jsonc`. Secretos requeridos para habilitar OAuth/Runner Control:

- `GITLAB_OAUTH_CLIENT_SECRET`: secreto de la aplicación GitLab Omnis Runner Control.
- `GITLAB_OAUTH_ALLOWED_USER_IDS`: IDs numéricos de los operadores permitidos.
- `OMNIS_OAUTH_ENCRYPTION_KEY`: clave Base64 de 32 bytes usada para cifrar credenciales.
- `OMNIS_RUNNER_DELIVERY_SECRET`: secreto aleatorio de alta entropía para entrega M2M.

`Cloudflare/configure_gitlab_oauth.sh` solicita el Client Secret en silencio, pregunta los IDs permitidos, genera el secreto M2M y guarda una copia local con permisos 600 bajo un directorio 700 fuera del repositorio. Si se vuelve a ejecutar, reutiliza esa copia para permitir reintentos. No regenera `OMNIS_OAUTH_ENCRYPTION_KEY`: hacerlo invalidaría las credenciales cifradas existentes.

Para rotar deliberadamente el bearer M2M, ejecutar `bash Cloudflare/rotate_runner_delivery_secret.sh` desde una máquina confiable. El script crea un valor nuevo, lo carga como secreto y reemplaza la copia local solo después de que Wrangler confirma el cambio. Esto no revoca tokens de runner emitidos anteriormente; si uno fue expuesto, revocar primero el runner en GitLab.

El script configura secretos pero **no** cambia `GITLAB_OAUTH_ENABLED` a true ni despliega. Para activar: revisar allowlist y callback; configurar todos los secretos; habilitar OAuth en una modificación revisada; ejecutar dry-run; aprobar expresamente un despliegue del Worker; comprobar `/health` y `/oauth/status`; probar OAuth y provisión en un entorno controlado. No activar OAuth en Previews.

Callback registrado:

`https://omnis-game.bazajhon.workers.dev/oauth/callback`

## Revocación y recuperación

Si se sospecha exposición del token de runner:

1. Pausar y revocar/eliminar el runner afectado en el proyecto GitLab.
2. Rotar `OMNIS_RUNNER_DELIVERY_SECRET` en Cloudflare y proteger la nueva copia local.
3. Tras confirmar la revocación remota, eliminar `gitlab:runner:credential` y `gitlab:runner:metadata` de KV mediante un procedimiento administrativo revisado.
4. Crear un runner sustituto solo tras revisar tags, executor, referencias protegidas y necesidad.

Si se expone el Client Secret OAuth, renovarlo en GitLab y actualizar `GITLAB_OAUTH_CLIENT_SECRET`. Para revocar OAuth, deshabilitar primero la variable, revocar el acceso de la aplicación en GitLab y retirar credenciales/sesiones de KV mediante procedimiento revisado.

No rotar `OMNIS_OAUTH_ENCRYPTION_KEY` sin migrar o eliminar previamente las credenciales cifradas que dependan de ella.

## Queue y observabilidad

El consumidor `queue()` registra solo cola, ID de mensaje, intentos y marca temporal. No serializa `message.body` ni campos arbitrarios. Esto reduce exposición en el consumidor; los productores, trazas, logs de aplicación y sistemas externos también deben aplicar minimización y redacción.

## Pruebas y límites

`npm run test:worker` ejecuta pruebas locales del código para cookie fija falsificada, allowlist, respuesta de provisión sin token, entrega M2M, sesión de estado, callback de identidad y logs de Queue sin cuerpo.

Estas pruebas no demuestran secretos, bindings ni despliegue en la cuenta. El Worker corregido está solo en la rama GitHub: no se ha desplegado, no se han creado runners, no se han leído valores secretos ni rotado credenciales.