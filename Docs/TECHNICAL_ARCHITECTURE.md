# Omnis — Technical Architecture

## Product

Omnis: El Último Ciclo.

## Runtime

C++ es la base de los sistemas runtime. UnrealBuildTool usa `.Build.cs` y `.Target.cs` para construir módulos y objetivos; Unreal Header Tool participa en la generación y validación de código reflejado.

## State graph

Core → Gameplay → World → Narrative → AI → Multiplayer/Online → UI/Audio/Camera.

Core mantiene estado transversal. Gameplay gobierna reglas. World gobierna simulación. Narrative mantiene consecuencias. AI consume contexto. Multiplayer/Online administra estado autoritativo. Presentation observa y presenta.

## Cycle contract

CycleIndex, Phase y WorldAge forman el contrato del ciclo. Nexo conserva progresión permanente. Save conserva solo el estado explícitamente persistible.
