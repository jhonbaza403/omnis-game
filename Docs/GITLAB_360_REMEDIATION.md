# Auditoría 360 de GitLab — Omnis: El Último Ciclo

Fecha de revisión: 2026-10-12. Proyecto GitLab: `omnis-game/omnis-game` (ID 87302401).

## Estado ejecutivo

El repositorio todavía no está certificado como build jugable. La rama principal identifica Unity 6.3 LTS (6000.3.0f1), pero el README contenía afirmaciones de Unreal Engine 5 y una rama de MR proponía Unity 7 Alpha. Esta corrección toma Unity 6.3 LTS como línea oficial de trabajo; Unity 7 Alpha no debe integrarse en esa línea.

El pipeline más reciente inspeccionado para la propuesta Unity 7 terminó con `stuck_pending_no_matching_runners`. Sus jobs no llegaron a ejecutarse: no equivale a una compilación fallida ni aprobada, y no hay artefactos de test/build que permitan certificar el juego.

## Bloqueos prioritarios

1. **Runners:** registrar y validar runners que coincidan con `omnis-linux-docker`; instalar Unity 6000.3.0f1 y Linux Build Support. Si se necesita la compilación Windows, registrar también un runner `omnis-windows`.
2. **Licencia:** configurar `UNITY_LICENSE` como variable CI/CD de tipo File, protegida y enmascarada cuando corresponda. Nunca pegar licencias o tokens en issues, logs o chat.
3. **Compilación real:** ejecutar importación/resolución de paquetes, compilación C#, pruebas PlayMode y build Linux dentro del Editor Unity exacto. El validador estructural no sustituye estos pasos.
4. **Gobernanza:** exigir pipeline exitoso y discusiones resueltas antes de fusionar. Configurar aprobación por Maintainer/revisor independiente si el plan de GitLab lo permite.
5. **Seguridad:** ejecutar secret detection, dependency scanning y revisión de permisos OAuth/Worker. Verificar que no haya credenciales en historial, artefactos o logs; rotar cualquier secreto expuesto.
6. **Publicación:** definir el destino oficial antes de eliminar configuración. GitLab Pages, Cloudflare y referencias históricas a Netlify coexisten; no ejecutar despliegues de producción sin confirmar destino y credenciales.
7. **Higiene del backlog:** usar issues con criterios de aceptación verificables y milestones para ordenar runners, motor, seguridad y publicación.

## Criterios de aceptación para certificar un build

- [ ] Runner con etiquetas correctas ejecutó jobs (no solo pendientes/skipped).
- [ ] Unity Editor 6000.3.0f1 importó el proyecto y resolvió paquetes sin errores.
- [ ] Compilación C# completa sin errores.
- [ ] Pruebas PlayMode ejecutadas y aprobadas con artefactos/logs.
- [ ] Build Linux generado y arrancado en un entorno de prueba.
- [ ] Secret/dependency scans revisados; no se publican secretos.
- [ ] README, versión de motor, CI y publicación describen el mismo flujo.
- [ ] Merge protegido por pipeline verde y revisión.

## Cambios de esta rama

- README actualizado para reflejar el estado real y la línea Unity 6.3 LTS.
- Job de build Linux ampliado a pipelines de Merge Request, además de la rama principal.
- Este documento registra los bloqueos y los criterios de salida.

## Limitaciones

Esta revisión no puede registrar runners, generar una licencia Unity, activar variables protegidas ni cambiar ajustes de seguridad del proyecto con las herramientas disponibles. Esas acciones requieren permisos/acciones en GitLab y, para la licencia, intervención del propietario de la cuenta. No se declara ningún test como aprobado sin evidencia del runner.
