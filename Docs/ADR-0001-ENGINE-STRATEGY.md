# ADR-0001 — Motor principal y estrategia de migración de Omnis

- **Estado:** decisión provisional para ejecución; requiere validar el vertical slice antes de retirar cualquier ruta.
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

1. **Unity será la ruta principal de desarrollo para el siguiente vertical slice**, porque la dirección de trabajo más reciente del proyecto se ha orientado a Unity 6.3/Unity 7 y ya hay una escena, scripts de gameplay y un punto de entrada de build Unity.
2. **Unity 6.3.0f1 será el baseline reproducible inicial** hasta que el editor y las dependencias del objetivo Unity 7 estén disponibles y la compilación automatizada pase. No cambiar `ProjectVersion.txt` solo por renombrar el objetivo.
3. **Unreal se conserva como implementación de referencia/legado** dentro del repositorio. No se borra, no se mueve y no se mezcla con Credi. Solo se podrá retirar tras comparar funcionalidades, recursos, licencias y el historial de commits, y tras una decisión expresa posterior.
4. La migración a Unity no se considera completada por tener scripts equivalentes: debe demostrar paridad funcional del vertical slice mediante tests, una build real y una prueba de lanzamiento.
5. La arquitectura de producción no puede llamarse “Unreal 7” hasta que haya versión oficial verificable, instalación reproducible y una CI que la compile.

## Por qué

Mantener dos motores activos como productos maestros duplica el coste de gameplay, arte, guardado, pruebas y publicación. Unity ya tiene una base de juego materializada en el repo y es la dirección de migración más reciente, por lo que es la ruta de trabajo preferida. Unreal conserva valor como fuente histórica hasta que la migración demuestre paridad.

## Criterios para promover Unity a producción

- [ ] Importación completa del proyecto en la versión de editor fijada.
- [ ] Cero errores de compilación C# y cero referencias de assets rotas.
- [ ] Tests automatizados pasan en CI.
- [ ] PlayMode tests pasan para movimiento, cámara, combate, interacción, Nexo, progresión, persistencia y derrota/respawn.
- [ ] Build Linux generada por Unity Editor real y artefacto asociado al SHA exacto.
- [ ] Smoke test del ejecutable con salida de proceso y logs archivados.
- [ ] Auditoría de assets/licencias y revisión de seguridad.
- [ ] Comparación de funcionalidades con la implementación Unreal y registro de diferencias.

## Consecuencias

- Nuevas funcionalidades del vertical slice se implementan primero en Unity.
- No se aceptan nuevas funcionalidades duplicadas en Unreal salvo correcciones de seguridad o mantenimiento estrictamente necesarias.
- La documentación y los pipelines deben etiquetar cada motor claramente.
- No se eliminan carpetas, plugins ni recursos de Unreal durante esta fase.
- El baseline solo cambia tras un build real exitoso y una actualización documentada de esta ADR.
