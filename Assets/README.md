# Omnis Unity 6.3

Unity 6.3 LTS runtime branch for Omnis: El Último Ciclo.

The existing Unreal Engine project remains intact. This Unity layer provides a real first playable vertical slice rather than an empty template.

Playable slice:
- Third-person exploration controller.
- Procedural Nexus environment, lighting and landmarks.
- Eight collectible cycle fragments.
- Portal objective and completion state.
- HUD with controls and progression.
- Reset/restart loop.

Unreal .uasset and .umap files are preserved and are not falsely declared as Unity assets. Source interchange assets can be migrated in a later asset-conversion pass.

Open the repository root from Unity Hub using Unity 6000.3.0f1. The initial scene is Assets/Scenes/OmnisPrototype.unity.
