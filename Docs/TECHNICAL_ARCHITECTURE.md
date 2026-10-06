# Omnis — Technical Architecture

## Scope

Omnis uses Unreal Engine 5 with a C++ runtime foundation and a data-driven content model. The architecture is expanded from verified gameplay requirements rather than folder count.

## Principles

- Runtime rules live in C++.
- Blueprints compose content and presentation where appropriate.
- GameInstance subsystems hold cross-world session state.
- Persistent and competitive state is validated by the authoritative server.
- Unreal binary assets use Git LFS.
- Generated local artifacts stay outside version control.
- A system is considered implemented only when code, integration and validation exist.

## Layers

Core → Gameplay → World → Narrative → AI → Multiplayer/Online → Presentation → Data/Systems.

## Vertical slice

BOOT → PRIMER CICLO → MUNDO → EXPLORACIÓN → NPC → DECISIÓN → COMBATE → FIN DEL CICLO → NEXO → MEJORA → NUEVO CICLO

Architecture grows from this verified flow.
