# Omnis — AAA Closeout Matrix

La regla de cierre es binaria: una capacidad está cerrada por evidencia real o permanece abierta. No se utilizan archivos vacíos, assets ficticios, builds inventadas ni “certificaciones” simuladas.

## Cerrado en repositorio

| Área | Evidencia actual |
| --- | --- |
| Unreal Engine | UE5.6 declarado y scripts de build fail-closed |
| Gameplay input | Enhanced Input como ruta única; legacy Axis/Action eliminado |
| Online common layer | Interfaces reales de OnlineSubsystem |
| Cloud persistence | Worker real + Supabase autenticado |
| Streaming runtime | Carga asíncrona con estado requested/loaded separado |
| Balance design | Datos versionados + validación + simulación de tuning |
| Vertical-slice narrative | Guion fuente original de seis beats |
| CI contracts | Validación automática + compileall |
| Provenance | Generator de manifest SHA-256 |
| Crash evidence | Collector de evidencia real disponible |
| Release gates | RC gate fail-closed |
| Console packaging | Jobs separados y fail-closed |

## Sigue abierto y no puede cerrarse solo con commits

| Área | Razón |
| --- | --- |
| Arte AAA final | Requiere producción profesional de assets UE |
| Mapas finales .umap | Requiere authoring real en Unreal Editor |
| Animación/rig/facial | Requiere assets y revisión visual |
| Audio/VO | Requiere producción, mezcla y QA de contenido |
| Cinematics finales | Requiere Level Sequences, captura/animación y edición |
| Localización final | Requiere traducción y QA lingüístico |
| Profiling PS5/Xbox/Switch 2 | Requiere SDK, runner y hardware autorizado |
| Providers propietarios | Requiere integración y credenciales del programa de cada plataforma |
| Store/submission | Requiere acceso de publicación y proceso externo |
| Certificación | Requiere ejecución de suites del fabricante y aprobación externa |

## Criterio final

El repositorio no declarará AAA certificado hasta que las filas abiertas tengan evidencia de producto y plataforma, no solamente código o documentación.
