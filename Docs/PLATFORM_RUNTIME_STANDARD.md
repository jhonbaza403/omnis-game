# Omnis — Unified Platform Runtime Standard

## Objetivo

Omnis mantiene una sola base de gameplay en C++/Unreal Engine 5.6. Las capacidades dependientes del fabricante se consumen mediante interfaces del Online Subsystem y una capa de plataforma común, sin duplicar lógica de gameplay.

## Capacidades implementadas en el runtime

| Capacidad | Implementación |
| --- | --- |
| Input y gamepad | Action/Axis mappings comunes, incluidos sticks, gatillos, D-pad, aceptar, cancelar, menú y atrás. |
| Save/load | USaveGame real, 10 slots, recuperación en transiciones de lifecycle y persistencia cloud autenticada. |
| Cuentas/perfiles | Identidad común y Online Subsystem; perfil cloud separado de Credi. |
| Achievements/trophies | Adaptador real sobre IOnlineAchievements. El proveedor autorizado de cada plataforma determina el catálogo y su publicación. |
| Online/multijugador | Sesiones, matchmaking, replicación y persistencia en módulos comunes; servicios de plataforma se conectan por sus implementaciones autorizadas. |
| Audio | Estado de audio común + respuesta al cambio de foco de audio del sistema. |
| Suspend/resume | Hooks de lifecycle y guardado preventivo con recuperación al volver al foreground. |
| DLC/entitlements | IOnlineEntitlements para consultar derechos del usuario; el delivery/patching final pertenece al sistema de distribución autorizado de cada plataforma. |
| Resolución/HDR | UGameUserSettings para resolución, HDR y resolución dinámica. |
| Escalabilidad | Perfiles mediante UGameUserSettings con niveles 0–4 y benchmark de hardware de Unreal. |
| Memoria | Métricas de memoria física disponibles/usadas/total mediante FPlatformMemory. |
| Streaming | Subsistema de streaming de Omnis separado de gameplay y preparado para World Partition/streaming de producción. |
| Telemetría | Eventos autenticados del juego hacia Cloudflare/Supabase y observabilidad del Worker. |
| Crashes/hangs | Hooks OnHandleSystemError/OnHandleSystemHang y crash markers locales. |
| Accesibilidad | Subtítulos, alto contraste, reducción de movimiento y asistencia de color. |
| Localización | Target de localización de Unreal con cultura nativa es y matriz de culturas objetivo. |
| Certificación | Checklist de release y gates P0–P4. La aprobación final depende del SDK, hardware, TRC/TCR y procesos del fabricante. |

## No simulación

No se incluyen SDKs propietarios falsos, binarios de consola inventados, logros publicados ficticios, DLC falsos ni resultados de certificación simulados.

La implementación de Omnis queda abierta a los providers oficiales mediante la misma capa de gameplay. La certificación solo se declara después de ejecutar las pruebas sobre hardware y herramientas autorizadas.

## Gates de Release Candidate

1. Build reproducible UE5.6 en runner autorizado.
2. Cook/package por plataforma.
3. Pruebas de lifecycle, save recovery, input, audio, video, memoria y streaming.
4. Pruebas online y de servicios de plataforma.
5. Pruebas de logros y entitlements con el backend real de cada plataforma.
6. Crash-free/hang-free en sesiones prolongadas.
7. Localización y accesibilidad validadas.
8. Verificación de requisitos vigentes del fabricante.
9. Cero blockers P0–P4.
