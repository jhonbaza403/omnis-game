# Omnis — PlayStation Certification Readiness

## Objetivo

Omnis se desarrollará con un estándar de producto de consola de alto nivel y tendrá PlayStation como plataforma objetivo de primera clase.

Esto no equivale a declarar certificación aprobada. Los requisitos finales de Sony Interactive Entertainment, TRC/TCR, SDK, submission tooling y procesos de certificación dependen del acceso autorizado del desarrollador y de la versión de plataforma correspondiente.

## Gate de producción

### P0 — Plataforma y SDK
- Obtener estatus oficial de desarrollador PlayStation.
- Obtener acceso autorizado al SDK y documentación de PlayStation.
- Alinear la versión exacta de UE 5.6 con el SDK aprobado para el proyecto.
- Crear un runner de compilación PlayStation autorizado y aislado.
- Mantener secretos, certificados y credenciales fuera del repositorio.

### P1 — Runtime de consola
- Input completo con mando.
- Navegación de interfaz con mando.
- Safe areas y legibilidad a distancia.
- Subtítulos configurables.
- Opciones de accesibilidad.
- Pause/resume y recuperación de estado.
- Guardado y recuperación robustos.
- Gestión de errores de red sin bloquear el juego.
- Pantallas de carga y recuperación de recursos.
- Limpieza de logs y datos sensibles.

### P2 — Rendimiento
- Objetivo primario: 60 Hz en el perfil de rendimiento.
- Perfil alternativo de calidad cuando el diseño lo requiera.
- Presupuestos explícitos de CPU, GPU, memoria, streaming, animación, VFX y audio.
- Perfilado sobre hardware real.
- Pruebas de sesiones prolongadas y recuperación de memoria.
- Prevención de shader stutter mediante precache y pruebas de PSO.
- Validación de World Partition/streaming bajo carga.

### P3 — QA
- Smoke tests por build.
- Functional tests.
- Regression suite.
- Save/load recovery.
- Suspend/resume.
- Input/controller matrix.
- Localization matrix.
- Accessibility matrix.
- Network interruption/recovery.
- Long-session stability.
- Performance budgets.
- Crash-free and hang-free certification gates.

### P4 — Submission
- Crear checklist exacto contra la documentación de PlayStation disponible al equipo.
- Ejecutar pruebas de conformidad de la versión objetivo.
- Corregir todos los blockers.
- Repetir en hardware final.
- Preparar paquetes de submission, metadatos, assets de store y documentación.
- Solo después de superar los gates internos se solicita la certificación oficial.

## Situación de Omnis

La arquitectura de código ya contiene subsistemas de juego, mundo, IA, narrativa, online, persistencia y UI. La rama de producción está ampliando esa base hacia consola, pero todavía no se puede declarar cumplimiento final de TRC porque aún no existe acceso al SDK privado ni una ejecución certificada sobre hardware PlayStation.

## Fuente pública

PlayStation Partners describe el programa como acceso a soporte, testing y herramientas de desarrollo. Epic indica que el acceso a la documentación específica de PlayStation 5 requiere aprobación de desarrollador, permisos y NDA; esa documentación no es pública en el sitio de documentación general de Unreal.

## Regla

No se implementará ninguna API propietaria de PlayStation basándose en suposiciones. Los puntos de integración específicos de Sony se encapsularán detrás de interfaces de plataforma y se implementarán únicamente con el SDK autorizado.
