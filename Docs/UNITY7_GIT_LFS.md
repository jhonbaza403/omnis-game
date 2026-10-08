# Unity 7 Git/LFS repository preparation

This repository remains the authoritative Omnis source repository. The current native project is Unreal Engine; this document prepares the repository for a real Unity 7 workspace without renaming or fabricating Unreal binaries.

## Target

- Unity: 7.0 Alpha 7000.0.0a7
- GitLab project: omnis-game/omnis-game
- Default branch: main
- Git LFS: enabled at the GitLab project level

## Unity repository rules

Track:
- Assets/ including every .meta file
- Packages/
- ProjectSettings/

Do not commit:
- Library/
- Temp/
- Obj/
- Build/
- Builds/
- Logs/
- MemoryCaptures/

Keep Unity YAML assets (.unity, .prefab, .asset, .mat, .anim, .controller, .meta) as normal Git text. Large binary media and source-model formats are routed through Git LFS.

## First local setup

From the Unity project root:

    git lfs install
    git lfs pull

Verify:

    git lfs env
    git lfs ls-files

Then open the project with Unity Hub using the exact Unity 7 Alpha editor version.

## Important

Do not copy Unreal .uasset or .umap files into Unity and rename them. Real Unity assets must be imported or created in Unity and committed with their corresponding .meta files.

## Current migration boundary

The repository may contain the existing Unreal project while the Unity workspace is being materialized. No fake conversion is performed by this repository configuration.
