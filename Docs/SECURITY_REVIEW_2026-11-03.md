# Omnis — revisión de seguridad y política de release

**Fecha:** 2026-11-03  
**Rama candidata:** `audit/omnis-360-baseline`  
**Producción:** sin cambios; no se desplegó ningún Worker.

## Decisión operativa

- **Repositorio fuente:** GitHub `jhonbaza403/omnis-game`.
- **Motor principal y único motor de release:** Unreal Engine 5.8 (`Omnis.uproject`).
- **Proyecto secundario conservado:** Unity 6.3 (`UnityProject/`). Sus validaciones de estructura son automáticas; los builds de referencia son manuales y no sustituyen ni bloquean el release Unreal.
- **CI oficial del juego:** GitHub Actions. La validación estática no reemplaza el workflow nativo UE 5.8.
- **GitLab CI:** auxiliar para validaciones, operaciones del Worker y builds de Unity de referencia.
- **Cloudflare Worker:** herramienta operativa auxiliar; su despliegue no publica ni certifica el videojuego.

Un release requiere, sobre el mismo SHA candidato: CI estático exitoso y workflow nativo exitoso con UE 5.8 (UHT/UBT, Automation, mapa/assets, empaquetado y smoke test de cliente y servidor). Un resultado estático verde no habilita publicación.

La rama `main` figuraba sin protección en la consulta de GitHub de esta revisión. Esta política está documentada, pero no se fuerza mediante branch protection. Antes de publicar hay que exigir PR review y checks requeridos. La puerta nativa aún no puede configurarse como check obligatorio hasta que exista un runner UE 5.8 conectado y el workflow produzca una ejecución verificable. No fusionar ni publicar mientras falte esa condición.

## Matriz de riesgos

| Riesgo | Evidencia comprobada | Tratamiento en la rama candidata |
|---|---|---|
| 1. CI estático no demuestra compilación Unreal | No se encontró evidencia de UHT/UBT, Automation, materialización, empaquetado o smoke test nativo en UE 5.8. | Se mantiene una puerta nativa separada. No se declara el juego compilado ni certificado. |
| 2. Unreal y Unity conviven en el repo | `Omnis.uproject` declara UE 5.8; `UnityProject/` declara Unity 6.3 y GitLab tenía builds. | UE 5.8 es la única fuente de verdad del release. Unity queda secundario; sus builds GitLab son manuales y `allow_failure`. |
| 3. Token del runner se enviaba al navegador | La versión anterior devolvía `registration_token` desde `/runner/provision`. | `/runner/provision` devuelve metadatos. Token cifrado en KV, TTL de 15 minutos y endpoint M2M con bearer separado. |
| 4. Autorización basada en cookie fija | La versión anterior aceptaba `omnis_oauth_connected=1`; Origin no autentica identidad. | Sesión opaca en KV, allowlist de ID GitLab, TTL 30 min, cookie HttpOnly/Secure/SameSite Lax y cierre M2M al deshabilitar OAuth, validación de mismo origen y logout. |
| 5. Queue registraba cuerpo completo | `queue()` serializaba `message.body`. | Logs limitados a cola, ID, intentos y marca temporal; no se registra el cuerpo. |
| 6. OAuth de producción no acreditado | La API de Cloudflare consultada el 2026-11-03 enumeraba solo `OMNIS_OAUTH_ENCRYPTION_KEY`; faltaban Client Secret, allowlist y delivery secret, y no aparecía `GITLAB_OAUTH_ENABLED` activo. | OAuth queda apagado por defecto en `wrangler.jsonc`; procedimiento de secretos/allowlist y activación documentado, sin desplegar. |

## Hallazgos adicionales de configuración

- El Worker `omnis-game` existe en Cloudflare; la última implementación registrada por API era del 2026-10-09 02:10 UTC. El código corregido de esta rama todavía no está desplegado.
- La consulta revisó nombres y tipos de secretos, no sus valores.
- El Worker crea runners con tag `omnis-ci-linux`, mientras `.gitlab-ci.yml` usa `omnis-linux-docker` como tag general. No hay evidencia de que el runner aprovisionado sea consumido por los jobs actuales. Alinear deliberadamente tag, executor, UE 5.8 y job consumidor antes de declararlo utilizable.
- La cuenta Cloudflare consultada reportaba `enforce_twofactor=false`. Se recomienda activar la exigencia de MFA/2FA; no se cambió la configuración de la cuenta.
- La observabilidad de Workers persiste logs y traces; el cambio evita serializar cuerpos en el consumidor Queue, pero los productores y sistemas externos también deben aplicar minimización y redacción.

## Credenciales del Worker

- No guardar credenciales en Git, `wrangler.jsonc`, issues, capturas, logs ni historial de shell.
- `GITLAB_OAUTH_CLIENT_SECRET`, `GITLAB_OAUTH_ALLOWED_USER_IDS` y `OMNIS_RUNNER_DELIVERY_SECRET` se gestionan como secretos del Worker.
- Se conserva la `OMNIS_OAUTH_ENCRYPTION_KEY` activa; rotarla sin migrar credenciales cifradas puede volverlas irrecuperables.
- La copia local del bearer M2M se guarda fuera del repo bajo directorio 700 y archivo 600. Rotarlo inmediatamente si se expone.
- El endpoint de entrega M2M devuelve el token de runner al cliente terminal autorizado; la ruta no es atómicamente de un solo uso y puede repetirse durante el TTL. El cliente debe evitar mostrar o registrar la respuesta.
- La revocación del runner debe realizarse en GitLab antes de retirar su credencial cifrada de KV. La renovación del Client Secret se realiza en GitLab y luego en Cloudflare.

## Límites y estado de esta intervención

Las pruebas Node del Worker son pruebas locales de código fuente. No prueban el despliegue, bindings ni secretos de la cuenta. En esta intervención no se desplegó Cloudflare, no se crearon ni revocaron runners, no se leyeron valores secretos, no se rotaron credenciales y no se modificó `main`.

Para certificar OAuth en producción todavía hace falta configurar los secretos por procedimiento aprobado, validar el ID de operador, habilitar OAuth mediante un cambio revisado, desplegar explícitamente, consultar `/health` y `/oauth/status`, completar OAuth con una identidad permitida y probar provisión/estado/revocación en entorno controlado.
