# Omnis — AAA Closeout Matrix

The closeout rule is binary: a capability is closed only when real evidence exists. Empty placeholders, fabricated binary assets, invented builds, simulated hardware results and self-declared certification are never accepted as evidence.

## Current evidence state

| Área | Estado real | Evidencia actual |
|---|---|---|
| Personaje/malla AAA final | 🔴 ABIERTO | No existen assets finales `.uasset`; el personaje sigue con scaffolding técnico. |
| Rig/facial/animación final | 🔴 ABIERTO | No existen assets finales de rig/animación/facial. |
| Assets PBR/VFX finales | 🔴 ABIERTO | No existen assets finales PBR/VFX. |
| Mapas `.umap` AAA finales | 🔴 ABIERTO | No existen mapas `.umap` finales en el repositorio. |
| World Partition sobre mapa final | 🟡 PARCIAL | Existe soporte/runtime de streaming y coordenadas; falta el mapa final authored en Unreal Editor. |
| Audio profesional/VO | 🔴 ABIERTO | No existen entregables de audio/VO finales. |
| Cinematics finales | 🔴 ABIERTO | No existen secuencias cinematográficas finales. |
| Localización final con QA lingüístico | 🟡 PARCIAL | Existe contrato/runtime de localización; falta QA lingüístico de lanzamiento. |
| Profiling PS5 | 🔴 ABIERTO | Requiere SDK + runner/hardware autorizado. |
| Profiling Xbox Series X/S | 🔴 ABIERTO | Requiere SDK + runner/hardware autorizado. |
| Profiling Switch 2 | 🔴 ABIERTO | Requiere SDK + runner/hardware autorizado. |
| Providers propietarios PSN/Xbox/Nintendo | 🔴 ABIERTO | Requiere acceso/credenciales y SDK de primera parte. |
| Store integration/submission | 🔴 ABIERTO | Requiere portales y proceso de publicación real. |
| Firma de distribución | 🔴 ABIERTO | Requiere infraestructura de firma de producción autorizada. |
| TRC/TCR y certificación | 🔴 ABIERTO | Requiere ejecución de suites y aprobación externa. |

## Lo que sí queda cerrado en repositorio

- Unreal Engine 5.6 y scripts de build fail-closed.
- Enhanced Input como ruta única de gameplay.
- OnlineSubsystem común y ciclo de sesiones.
- Persistencia cloud real con Worker + Supabase autenticado.
- Streaming asíncrono con estado solicitado/cargado separado.
- Datos de balance/narrativa versionados y validados.
- Contratos de CI, provenance y evidencia de crash.
- Jobs separados para build/empaquetado de plataformas.
- Auditoría automática de evidencia de contenido mediante `Tools/Release/audit_content_delivery.py`.

## Regla de cierre

El repositorio no declarará un producto AAA certificado hasta que las filas abiertas tengan evidencia de producto, plataforma y/o certificación aplicable. La implementación de código ya realizada no sustituye la producción de contenido ni las validaciones de hardware/primera parte.
