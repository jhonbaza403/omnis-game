# Omnis — Matriz QA multiplataforma

## Objetivo

Validar la misma base de gameplay y las mismas reglas de producto en cada plataforma, cambiando solo los adaptadores autorizados y los parámetros de hardware.

| Área | Caso | Resultado esperado | Evidencia |
| --- | --- | --- | --- |
| Input | Stick izquierdo/derecho, gatillos, D-pad, aceptar/cancelar/menu/atrás | Todas las acciones funcionan sin teclado | Captura de input test |
| Identidad | Login, cambio de usuario, reanudación | Saves, logros y rewards siguen asociados al usuario activo | Log de identidad |
| Save | Crear/cargar/borrar slots 1–10 | Estado consistente y versionado | Save/load report |
| Suspend | Entrar background/deactivate y volver | Guardado preventivo y recuperación | Lifecycle log |
| Online | Buscar y unirse a sesión | Sesión publicada, búsqueda filtrada por juego/playlist y client travel | Session log |
| Achievements | Reportar progreso/desbloqueo | Provider acepta el achievement y conserva su asociación al perfil | Provider result |
| Entitlements | Refresh y consulta de DLC | Solo contenido autorizado queda disponible | Entitlement report |
| Updates | Enumerar/leer title content | Archivos accesibles desde el provider autorizado | Update log |
| Audio | Perder/recuperar audio focus | Audio se atenúa/restaura sin bloquear el juego | Audio focus log |
| Video | HDR, resolución dinámica, FPS | Ajustes aplicados y guardados | Video profile |
| Memory | Bajo/alto uso de memoria | Perfil adaptativo reduce carga antes de llegar al límite crítico | Memory samples |
| Streaming | Cargar/liberar regiones | No exceder máximo de regiones residentes; sin referencias retenidas | Streaming report |
| Telemetry | Evento válido/inválido | Válido autenticado; inválido rechazado sin datos sensibles | API response |
| Crash/hang | Error/hang controlado en build de test | Marker local + evento cloud cuando exista conectividad | Crash artifacts |
| Accessibility | Subtítulos, contraste, movimiento, color | Opciones permanecen funcionales durante todo el flujo | Accessibility matrix |
| Localization | Cambiar cultura y reiniciar | Cultura válida aplicada; cultura no soportada rechazada | Localization report |
| Long session | Sesión prolongada | Sin crecimiento no controlado de memoria ni pérdida de estado | Performance capture |
| Recovery | Interrupción de red | Juego continúa localmente y recupera cloud cuando vuelve la conectividad | Network recovery report |

## Matriz de evidencia

Cada caso de consola debe conservar:

- versión del commit;
- versión exacta de UE 5.6;
- versión del SDK/GDK autorizado;
- configuración del dispositivo;
- build ID;
- resultado;
- logs;
- capturas de rendimiento;
- defectos y correcciones.

## Regla

Una prueba automatizada de escritorio valida el contrato del código. Solo una ejecución en el hardware objetivo valida el comportamiento final de consola.
