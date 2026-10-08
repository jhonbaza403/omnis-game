# Omnis Unity 7 materialization

## Current state

The Unity workspace is being materialized as a genuine Unity project alongside the existing Unreal project.

- Editor target: Unity 7 Alpha 7000.0.0a7
- Source repository: `omnis-game/omnis-game`
- Source geometry: original Git LFS assets under `Content/Omnis/SourceAssets/`
- Unity generated folders remain ignored.
- Unreal assets remain untouched.

## Integrity rule

The recovered Git LFS object for `Content/Omnis/SourceAssets/Textures/Omnis_Nexus_Albedo.png` passed `git lfs fsck` locally before this materialization step.

No synthetic replacement is being substituted for that source asset.

## Next verified milestone

Open the `unity7-materialization` branch with Unity Hub using 7000.0.0a7, allow Unity to generate native project metadata, then import the verified source models and texture through Unity's importer.
