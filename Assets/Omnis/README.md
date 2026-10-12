# Omnis — Unity 7

This is the real Unity workspace for **Omnis: El Último Ciclo**.

## Boundary

- Unreal Engine project files remain authoritative in their existing locations.
- Unity assets are created/imported as native Unity assets.
- Unreal `.uasset` and `.umap` files are never renamed or presented as Unity assets.
- Source geometry is imported from the original repository assets under `Content/Omnis/SourceAssets/`.
- Unity-generated `.meta` files must be committed with their corresponding source assets.

## First materialization target

1. Import verified source models.
2. Create native Unity materials and prefabs.
3. Build the first playable Omnis scene.
4. Verify the project with Unity 7 Alpha 7000.0.0a7.
