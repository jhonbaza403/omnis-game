# ADR-0001 — Motor principal y estrategia de migración de Omnis

- **Estado:** decisión de motor aceptada; la promoción a producción depende de las pruebas de aceptación.
- **Fecha:** 2026-10-28
- **Proyecto:** Omnis: El Último Ciclo
- **No afecta:** Credi Marketplace, cuyo repositorio debe permanecer independiente.

## Contexto verificado

- La rama principal de GitHub contiene una base C++ de Unreal y un proyecto Unity en `UnityProject/`.
- `Omnis.uproject` declara `EngineAssociation: 5.8`.
- `UnityProject/ProjectSettings/ProjectVersion.txt` declara `6000.3.0f1` (Unity 6.3).
- La CI de GitLab en `main` ha fallado con `stuck_pending_no_matching_runners`; los jobs listados no tenían runner asignado. Esto impide usar el estado de CI como prueba de compilación.

## Decisión de trabajo

1. **Unreal Engine es el motor principal de Omnis: El Último Ciclo.** Las nuevas funcionalidades jugables se implementarán en la base Unreal existente; no se mantendrán dos implementaciones maestras activas.
2. **Unreal Engine 5.8 es la versión de referencia declarada actualmente**, porque `Omnis.uproject` declara `EngineAssociation: 5.8`. Antes de cambiar de versión, se verificará la instalación exacta disponible y se ejecutarán compilación y pruebas reales.
3. **Unity queda como proyecto secundario de referencia**, sin nuevas funcionalidades duplicadas. Sus archivos se conservan; no se borra ni se convierte automáticamente ningún recurso.
4. No se declara “Unreal Engine 7” como objetivo: esta auditoría no verificó una versión oficial y compilable con ese nombre. La decisión es usar la base Unreal existente y validar la versión concreta del editor disponible.

## Por qué

Mantener dos motores activos como productos maestros duplica el coste de gameplay, arte, guardado, pruebas y publicación. Unreal concentra el desarrollo futuro porque ya existe una base C++ y un archivo `Omnis.uproject`. Unity permanece intacto como proyecto secundario, sin nuevas funcionalidades duplicadas. Esta decisión no equivale todavía a una compilación validada: hace falta ejecutar Unreal Editor/Build Tool y las pruebas de gameplay en un entorno real.

## Criterios para validar Unreal como motor principal

- [ ] Confirmar la instalación real y la ruta de Unreal Engine 5.8 disponible para el proyecto.
- [ ] Generar/validar archivos de proyecto y compilar `OmnisEditor` con Unreal Build Tool.
- [ ] Resolver errores reales de C++ y módulos/plugins sin ocultar ni desactivar pruebas.
- [ ] Ejecutar Unreal Automation Tests para movimiento, cámara, combate, interacción, Nexo, progresión, persistencia y derrota/respawn.
- [ ] Generar un build Linux reproducible y vincular artefacto y logs al SHA exacto.
- [ ] Ejecutar smoke test del binario y archivar el resultado.
- [ ] Auditar assets/licencias, dependencias, secretos y permisos de CI.
- [ ] Documentar Unity como proyecto secundario y evitar nuevas funcionalidades duplicadas.

## Consecuencias

- Nuevas funcionalidades del vertical slice se implementan en Unreal Engine.
- Unity se conserva como proyecto secundario, sin nuevas funcionalidades duplicadas salvo una tarea de migración expresamente autorizada.
- La documentación y los pipelines deben etiquetar cada motor claramente.
- No se eliminan carpetas, plugins ni recursos de Unreal o Unity durante esta fase.
- La versión de motor solo cambia tras un build real exitoso y una actualización documentada de esta ADR.
