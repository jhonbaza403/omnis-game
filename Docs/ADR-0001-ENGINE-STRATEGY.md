# ADR-0001 — Motor principal y estrategia de migración de Omnis

- **Estado:** aceptada como estrategia de desarrollo; la promoción a producción depende de las pruebas de aceptación.
- **Fecha:** 2026-10-28
- **Proyecto:** Omnis: El Último Ciclo
- **No afecta:** Credi Marketplace, cuyo repositorio debe permanecer independiente.

## Contexto verificado

- La rama principal de GitHub contiene una base C++ de Unreal y un proyecto Unity en `UnityProject/`.
- `Omnis.uproject` declara `EngineAssociation: 5.8`.
- `UnityProject/ProjectSettings/ProjectVersion.txt` declara `6000.3.0f1` (Unity 6.3).
- La CI de GitLab en `main` ha fallado con `stuck_pending_no_matching_runners`; los jobs listados no tenían runner asignado. Esto impide usar el estado de CI como prueba de compilación.
- No se ha encontrado en esta revisión una versión oficial de Unreal Engine 7 que pueda utilizarse como objetivo reproducible de compilación.

## Decisión de trabajo

1. **Unity es el único motor principal para las nuevas funcionalidades de Omnis.** No se mantendrán dos implementaciones maestras activas.
2. **Unity 6.3.0f1 es el baseline estable y reproducible actual** porque esa es la versión fijada en `UnityProject/ProjectSettings/ProjectVersion.txt`. Se conserva hasta demostrar una migración compilable y probada.
3. **Unity 7 Alpha es el objetivo de evaluación de migración, no el baseline de producción.** Se probará en una rama de migración separada, sin modificar el baseline estable, y solo se promoverá tras importación limpia, compilación, PlayMode tests y build Linux reproducible.
4. **Unreal se conserva como referencia histórica/legado**, sin nuevas funcionalidades de producto salvo mantenimiento crítico. No se borra ni se mueve ningún archivo de Unreal durante esta fase.
5. No se declara “Unreal 7” como objetivo: no hay evidencia verificada en esta auditoría de una versión oficial y compilable con ese nombre. La dirección elegida es Unity, no Unreal.

## Por qué

Mantener dos motores activos como productos maestros duplica el coste de gameplay, arte, guardado, pruebas y publicación. La dirección queda cerrada: Unity concentra el desarrollo futuro; Unity 6.3.0f1 protege la reproducibilidad actual y Unity 7 Alpha se evalúa por separado. Unreal permanece intacto como referencia histórica hasta que se confirme la paridad y se autorice expresamente cualquier retirada.

## Criterios para promover Unity a producción

- [ ] Importación completa del proyecto en Unity 6.3.0f1 (baseline estable).
- [ ] Cero errores de compilación C# y cero referencias de assets rotas.
- [ ] Tests automatizados pasan en CI.
- [ ] PlayMode tests pasan para movimiento, cámara, combate, interacción, Nexo, progresión, persistencia y derrota/respawn.
- [ ] Build Linux generada por Unity Editor real y artefacto asociado al SHA exacto.
- [ ] Unity 7 Alpha evaluado en rama aislada, sin reemplazar el baseline antes de superar las pruebas.
- [ ] Smoke test del ejecutable con salida de proceso y logs archivados.
- [ ] Auditoría de assets/licencias y revisión de seguridad.
- [ ] Comparación de funcionalidades con la implementación Unreal y registro de diferencias.

## Consecuencias

- Nuevas funcionalidades del vertical slice se implementan primero en Unity.
- No se aceptan nuevas funcionalidades duplicadas en Unreal salvo correcciones de seguridad o mantenimiento estrictamente necesarias.
- La documentación y los pipelines deben etiquetar cada motor claramente.
- No se eliminan carpetas, plugins ni recursos de Unreal durante esta fase.
- El baseline solo cambia tras un build real exitoso y una actualización documentada de esta ADR.
