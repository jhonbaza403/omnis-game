# Auditoría técnica e integración inicial — OMNIS
**Fecha de corte:** 2026-11-24  
**Repositorio:** \`jhonbaza403/omnis-game\`  
**Rama de trabajo:** \`audit/omnis-360-baseline\`  
**PR única de trabajo:** #1 (se mantiene Draft)  
**Motor principal:** Unreal Engine 5.8  
**Producción/main:** no modificada.

## 1. Estado del repositorio inspeccionado

La inspección del árbol Git de \`main\` encontró 707 entradas, 498 archivos, 211 archivos bajo \`Source/\`, seis plugins Omnis habilitados y un proyecto Unity 6.3 secundario. En \`Content/Omnis/SourceAssets\` hay 63 OBJ y 63 MTL. No hay \`.uasset\` ni \`.umap\` nativos en el árbol de \`main\`. Un OBJ inspeccionado es un puntero Git LFS, no el modelo materializado.

El árbol demuestra presencia de fuentes y catálogos, no certifica que el juego arranque ni que los sistemas estén integrados en runtime.

## 2. Hallazgos verificados

| Dominio | Estado real encontrado | Brecha principal |
|---|---|---|
| Core/ciclo/tiempo | Existen subsistemas. El reloj solo acumula segundos y escala temporal. | Falta calendario de juego, reloj persistente y eventos temporales de mundo. |
| Misiones | El subsistema anterior solo guardaba un enum por ID. | Sin flujo completo de inicio, éxito/fallo, intentos, consecuencias ni recuperación. |
| Consecuencias | Un conjunto de IDs activos. | Sin aplicación coordinada a NPC, facciones, reputación y mundo. |
| Personajes/NPC | El NPC anterior tenía movimiento oscilatorio y entregaba XP al interactuar. | Sin rutinas, necesidades, memoria, relaciones usadas por IA o persistencia. |
| Decisiones/diálogo | Registro de decisiones y diálogo lineal por líneas. | Sin condiciones/ramificaciones conectadas al estado del mundo y las consecuencias. |
| Facciones/reputación | Mapas de puntuaciones modificables. | No se disparaban desde un resultado de misión real. |
| Guardado | \`UOmnisSaveGame\` conserva estado del ciclo y puntos permanentes. | No serializa misiones, consecuencias, relaciones ni memoria/vida de NPC. |
| Plugins | Los seis módulos Omnis tienen descriptores y módulos de arranque vacíos; cada Build.cs declara solo Core. | Son puntos de extensión, no seis subsistemas funcionales. La propiedad funcional sigue en \`Source/Omnis\`. |
| Contenido 3D | Fuentes OBJ/MTL y catálogos. | Materialización nativa UE5.8 pendiente; el mapa y los assets no están en Git como binarios Unreal. |
| CI/build | Workflows manuales para runners propios UE5.8. | No hay evidencia de UHT/UBT, Automation Tests, cook/package ni smoke test ejecutados en el commit actual. |

## 3. Cambios integrados en este corte

### Misiones con éxito, fracaso y reparación
- Añadida una API explícita \`StartQuest\`, \`CompleteQuest\` y \`FailQuest\`.
- El subsistema rechaza iniciar una misión ya activa o completada y evita finalizar dos veces una misión terminal.
- Cada misión puede configurar su propia consecuencia de fracaso, penalización de reputación, impacto en una facción, contador/estado del mundo, testigos y misión de recuperación.
- El fracaso registra intento y motivo, aplica el ID de consecuencia, actualiza reputación/facción/estado del mundo y guarda recuerdos en el jugador y en los NPC testigos.
- El éxito otorga XP y emite un evento de misión completada.
- Se publica un delegado de transición para que HUD, diálogo y otros sistemas puedan reaccionar sin duplicar la propiedad del estado.

El preset por defecto usa \`Mission.Failure.PublicShame\`, reputación -10 y relación de testigos -15. El diseño de cada misión debe definir testigos y, cuando corresponda, facción y recuperación.

### Personajes con estado y rutinas
- El componente de estado del personaje ahora contiene hambre, energía, necesidad social, relaciones acotadas, recuerdos con importancia y objetivo vital.
- Los recuerdos se limitan a las 32 entradas más recientes para evitar crecimiento ilimitado.
- Player y NPC reciben el componente de estado.
- Los NPC usan un horario básico según el reloj de mundo, eligen trabajar, comer, descansar, socializar o explorar, y solicitan movimiento mediante AIController.
- Las interacciones generan memoria y cambian la relación. Un NPC con relación suficientemente negativa puede rechazar una interacción posterior.
- La muerte de un NPC deja el actor marcado como muerto en lugar de destruirlo inmediatamente; el comportamiento vital se detiene. Los jugadores conservan el comportamiento previo de respawn/despawn.

### Validación añadida
Se añade un archivo de Automation Tests C++ para las reglas de necesidades/memoria/relaciones y para las transiciones de fracaso/recuperación. Se amplía la prueba Python de narrativa con contratos de fuente. Las pruebas Python son validaciones estáticas, no sustituyen los Automation Tests dentro de Unreal.

## 4. Limitaciones que no deben declararse resueltas

1. **Persistencia real:** ya existe una API versionada de guardado/carga y snapshots de NPC, pero no está certificada en Unreal 5.8. Todavía no restaura NPC ausentes en regiones no cargadas ni simula vida/economía mientras la aplicación está cerrada.
2. **Contenido autorado:** cada NPC colocado necesita un \`CharacterId\` único, hogar y \`WorkLocationOffset\` revisados; cada misión necesita un Data Asset configurado con testigos, facciones y consecuencias. No se crean personajes narrativos ni diálogos personalizados automáticamente.
3. **NPCs de vida y navegación:** agenda/objetivos, necesidades y memoria existen en cada componente, pero dependen de NavMesh válido y de un PersistentId único asignado a cada instancia. Sin NavMesh, el movimiento puede no completarse.
4. **Vergüenza social completa:** los testigos guardan el ID de consecuencia como recuerdo y su prompt de interacción puede reaccionar a Mission.Failure.PublicShame; el actor de mundo replica consecuencias y puntuaciones. La propagación emergente de rumores, reacciones de multitudes y ramificación de diálogo más rica todavía requieren diseño e integración.
5. **Calendario:** el reloj es relativo al tiempo transcurrido (una hora de juego por 60 segundos simulados, con escala 1). No es todavía un calendario persistente con estaciones, días y eventos globales.
6. **Plugins:** no se duplicaron los sistemas en los plugins vacíos. Una extracción futura requiere mover propiedad de código en un único cambio controlado.
7. **Arte:** es obligatorio hidratar Git LFS y materializar/guardar assets y mapa desde Unreal Engine 5.8 real.
8. **Certificación:** no se ejecutaron aquí UnrealHeaderTool, UnrealBuildTool, Unreal Automation, cook/package, cliente ni servidor dedicado. No se declara el build certificado.

## 5. Arquitectura de referencia para la siguiente integración

\`QuestDefinition → QuestSubsystem → Consequence/Reputation/Faction/WorldState → Witness CharacterState → NPC behavior/Dialogue/UI\`

\`World Time → NPC needs + schedule → Navigation → interaction/social memory → CharacterState persistence\`

\`SaveGame/Repository persistence\` debe capturar de forma versionada misiones, consecuencias, estado del mundo y snapshots de NPC, con autoridad de servidor para multiplayer y migraciones seguras.

## 6. Seguridad del cambio

- La rama de trabajo se sincronizó con el SHA vigente de \`main\` antes de integrar este corte.
- \`main\` permanece intacta.
- Se conserva la PR #1 como única línea de revisión; no se abre una rama ni PR adicional.
- No se borra contenido ni se fabrican binarios \`.uasset/.umap\`.
- Los cambios de gameplay permanecen en la ubicación canónica \`Source/Omnis\`; los plugins siguen siendo extensiones, no copias.

## 7. Criterio de aceptación para certificar

- UHT/UBT compilan Editor, Game y Dedicated Server en UE5.8.
- Unreal Automation pasa los tests \`Omnis.Narrative.CharacterLifeState\` y \`Omnis.Narrative.QuestFailureConsequences\`.
- Prueba funcional: fallar una misión produce estado Failed, reputación/facción/estado de mundo y recuerdos de testigos; completar la recuperación retira solo la consecuencia prevista si así se configura.
- NPC de prueba completa ciclos trabajo/comida/descanso/socialización, recuerda al jugador, cambia relación y aplica rechazo por relación baja.
- Guardar, cerrar, volver a abrir y cargar conserva los estados de misión y NPC.
- El mundo se materializa con assets LFS reales; cliente y Dedicated Server pasan smoke tests.
- No se integra a producción hasta cerrar evidencia de los gates anteriores.

**Resultado:** auditoría e integración inicial de gameplay en rama de revisión. No es todavía una certificación de juego completo ni de build Unreal.

## 8. Segunda integración — persistencia de partida y reparación de consecuencias

**Código añadido en la revisión posterior de la misma PR:**

- \`UOmnisSaveGame\` usa esquema versionado 2 y almacena fecha UTC, ciclo, Nexo, reloj de mundo, progreso/intent os de misión, consecuencias activas, reputación, facciones, estado global, variables persistentes y snapshots de personajes.
- \`UOmnisPersistenceSubsystem\` incorpora \`SaveCurrentGame\` y \`LoadCurrentGame\` usando los slots estándar de Unreal. El guardado/carga se rechaza en clientes de red; la autoridad del servidor mantiene el control del estado compartido.
- Los snapshots de personajes contienen identidad persistente, transform, necesidades, relaciones, recuerdos, estado vital y salud actual/máxima.
- Los NPC colocados usan como clave fallback su ruta de actor estable; los NPC generados dinámicamente deben recibir un \`PersistentId\` explícito y único. \`CharacterId\` continúa siendo el tipo/arquetipo narrativo.
- Se agregan exportación/importación versionada para progreso de misiones, consecuencias, reputación, facciones, Nexo, ciclo y reloj.
- Una misión de recuperación puede retirar consecuencias concretas, restituir reputación y facción, revertir contadores del mundo con límite inferior cero y recuperar relaciones de testigos.
- Se incorporan pruebas de Automation para ida/vuelta de snapshots y pruebas estáticas de los contratos de guardado/carga.

### Lo que esta integración todavía no certifica

1. El código de SaveGame aún no se ha ejecutado dentro de Unreal 5.8; falta comprobar escritura real en disco, lectura, migraciones y compatibilidad de serialización.
2. El guardado manual expone API BlueprintCallable, pero falta conectarlo a menú/HUD, autosave, selección de slots y política de respaldo.
3. El mundo compartido se guarda desde autoridad, pero la persistencia de identidades de cuenta multijugador y restauración de sesiones de varios jugadores exige enlazar IDs online estables.
4. Los NPC colocados necesitan IDs únicos y NavMesh válido. Los NPC dinámicos requieren asignación explícita de PersistentId por su spawner.
5. El reloj se conserva entre sesiones; la simulación offline de necesidades, economía, población y eventos mientras el juego está cerrado todavía no está implementada.
6. El sistema social no difunde rumores de forma emergente a toda una población. En este corte, testigos explícitos recuerdan el hecho y ajustan su relación.
7. Los Automation Tests C++ son nuevos, pero requieren ejecutarse con Unreal Engine 5.8 para considerarse aprobados.

**Estado actualizado:** persistencia de snapshots y reparación de consecuencias implementadas en código de la rama candidata; pruebas nativas y certificación real siguen pendientes.

## 9. Tercera integración — autoridad, replicación y lectura de consecuencias en clientes

- La interacción deja de ejecutarse en el cliente local: el componente replica y solicita ServerInteract; el servidor vuelve a trazar, limita distancia y ejecuta la interfaz del objetivo.
- El ataque usa ServerAttack, valida alcance/daño y aplica daño únicamente desde autoridad.
- Retirar una pieza de construcción usa ServerRemoveLastPiece; la lista de piezas construidas se replica y el servidor conserva el control del borrado.
- La posesión de vehículos y aeronaves se solicita al servidor mediante RPC desde el PlayerController.
- El estado de vida, recuerdos, necesidades y relaciones se replica mediante propiedades explícitas; las relaciones se transmiten como registros en array y se reconstruye el mapa local en clientes.
- Se reutiliza AOmnisReplicatedWorldStateActor como puente de lectura para misiones, consecuencias, reputación, facciones y contadores del mundo; emite OnNarrativeStateChanged al actualizarse.
- Los NPC simulan necesidades y rutinas en autoridad y replican estado/movimiento a clientes.
- Los identificadores persistentes de NPC ahora son explícitos; no se usa un nombre de actor generado en runtime como identidad duradera. El guardado falla de forma segura si falta o se duplica el ID.
- Se acepta el esquema legado 0 y se migra el campo de puntos permanentes al esquema 2; otros esquemas desconocidos se rechazan antes de modificar el estado de partida.
- Los dos NPC creados por el mundo bootstrap reciben `NPC.Bootstrap.01` y `.02` antes de `BeginPlay`; NPC adicionales deben recibir un `PersistentId` único explícito.
- Reputación y facciones se acotan a [-100, 100], también al importar un guardado. El estado de jugadores online sigue necesitando un identificador de cuenta estable por jugador; la instantánea de jugador actual corresponde al índice local 0.

### Aún pendiente antes de certificación

- Validación UHT/UBT de los RPC nuevos, ReplicatedUsing, arrays de structs y firmas de delegates.
- Validar en servidor dedicado + dos clientes que interacción, daño, construcción, posesión y estado narrativo se ven correctamente.
- Conectar HUD/diálogo al actor replicado y diseñar la difusión de rumores entre testigos con radio/decadencia.
- Asignar PersistentId a todos los NPC colocados y spawneados; comprobar IDs únicos en contenido de producción.
- Cargar mundo/actores antes de LoadCurrentGame; hoy la carga aborta si el NPC guardado no está en la región/level activo.
- Integrar autosave, selección de slots y migraciones adicionales; API manual ya disponible.
- Ejecutar pruebas Python y Automation C++ en CI/UE5.8; estática no equivale a runtime.

Resultado actualizado: los caminos principales de interacción, combate y construcción se encaminan a autoridad de servidor; persistencia y estado narrativo replicado ya están en la rama candidata. Todavía no se certifica multiplayer ni build Unreal.
