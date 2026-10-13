# OMNIS: El Último Ciclo

**Estado del proyecto:** en transición y no certificado todavía como build jugable.

## Motor oficial

La línea de trabajo actual usa **Unity 6.3 LTS (6000.3.0f1)**. La versión del Editor está declarada en `UnityProject/ProjectSettings/ProjectVersion.txt`. No se debe mezclar esta línea con Unity 7 Alpha ni declarar validada una versión que no haya pasado importación, resolución de paquetes, compilación, pruebas y build real en Unity Editor.

Hay contenido y documentación heredados de Unreal Engine en el repositorio. Su presencia no significa que formen parte del build oficial de Unity ni que se hayan convertido automáticamente. Deben inventariarse por separado antes de migrarlos o retirarlos.

## Identidad y experiencia

**Omnis: El Último Ciclo** combina exploración, construcción, narrativa, combate, progresión y un ciclo de renacimiento denominado Nexo. Su dirección artística es dark fantasy postapocalíptica: ruinas monumentales, naturaleza invasiva, tecnología perdida y fenómenos extraordinarios.

El objetivo jugable contempla cooperación de escuadra, desafíos competitivos, personajes y regiones persistentes. Estas metas no deben describirse como funcionalidades terminadas sin pruebas reproducibles.

## Reglas de ingeniería

- No fabricar archivos nativos, capturas ni resultados para aparentar una migración o build.
- No integrar una rama hasta que las comprobaciones requeridas hayan terminado correctamente.
- No guardar secretos, tokens, licencias ni credenciales en el repositorio o en logs.
- Mantener el motor y las versiones de paquetes fijados y verificables.
- Distinguir validaciones estructurales de compilación real y pruebas ejecutadas dentro de Unity Editor.
- Mantener los despliegues desactivados/manuales hasta validar el destino y las credenciales.

## CI y validación

El proyecto usa GitLab CI. Los jobs que requieren Unity necesitan un runner compatible con las etiquetas declaradas y una licencia válida configurada en CI/CD Variables. Si el job queda pendiente por falta de runner, no se considera que la compilación o los tests hayan pasado.

El validador estructural en `CI/Unity/validate_project.sh` no sustituye la importación de paquetes, la compilación C# ni las pruebas PlayMode. Consulta `Docs/GITLAB_360_REMEDIATION.md` para los bloqueos y criterios de cierre de la auditoría.
