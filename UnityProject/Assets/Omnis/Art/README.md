# Omnis Unity art intake and visual acceptance

The current vertical slice can generate a deterministic playable environment, but procedural primitives are only a fallback. They are not a substitute for production character models, skeletal animation, physically based material sets, or authored vegetation.

## Import locations

- `Imported/Characters`: rigged humanoid/enemy FBX prefabs with a working humanoid skeleton, skin weights, LODs, materials, and licensed animation clips.
- `Imported/Environment`: modular ruins, gates, props, rocks, and architecture with collision and sensible pivots.
- `Imported/Nature`: distinct tree/plant meshes, trunks, branches, foliage materials, LODs, and wind-compatible shaders.
- `Imported/Materials`: physically based material sets with base-color, normal, roughness/smoothness, metallic and ambient-occlusion maps where appropriate.

## Mandatory intake checks

1. Retrieve Git LFS objects first; a pointer text file is not an asset.
2. Record the source URL, creator, license, attribution requirement, source revision, and import date for each third-party asset.
3. Confirm each mesh is imported by Unity and opens in the Inspector; check scale, normals, tangents, materials, UVs, rig, animation and collision.
4. Run **Omnis → Art → Audit Real Production Assets**. It emits `TestResults/omnis-art-readiness.json`.
5. Review the imported assets in the scene at 4K and at 1080p; capture before/after screenshots and profile on the target GPU.

## Visual target

Grounded cinematic dark fantasy, not comic-book/cel-shaded art. Use realistic proportions and surface response, restrained emissive accents, varied vegetation silhouettes, believable terrain transitions, soft but legible shadows, fog with depth cues, and a restrained color grade. Do not claim photorealism merely because the display output is 3840×2160.

The audit deliberately reports blockers instead of passing when the project contains only procedural fallback geometry. A green report is a gate to visual review, not proof of final quality or performance.
