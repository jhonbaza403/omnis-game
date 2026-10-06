# Omnis — AAA Content Delivery Specification

This document is the production handoff contract for the assets that cannot be truthfully completed by repository code alone.

## Evidence rule

No empty placeholder, fabricated binary, generated checksum, simulated hardware result, or self-declared certification closes a release gate. A gate closes only after the actual deliverable exists and the applicable QA/platform evidence is recorded.

## Required production deliveries

| Gate | Required real evidence | Repository location |
|---|---|---|
| Character / mesh AAA | Final skeletal mesh, materials, LODs, collision, playable-character integration, content QA | `Content/Omnis/Characters`, `Content/Omnis/Meshes` |
| Rig / facial / animation | Final skeleton/rig, locomotion/combat animation set, facial setup, animation QA | `Content/Omnis/Animations`, `Content/Omnis/Characters` |
| PBR / VFX | Final textures/material instances, VFX, references from final gameplay assets | `Content/Omnis/Materials`, `Content/Omnis/Environment`, `Content/Omnis/VFX` |
| Maps / World Partition | Final `.umap` maps, World Partition/Data Layers/HLOD authored in Unreal Editor | `Content/Omnis/Maps` |
| Audio / VO | Final mastered SFX/music/ambience and approved VO with localization coverage | `Content/Omnis/Audio` |
| Cinematics | Final Level Sequences, camera/animation/audio integration and editorial QA | `Content/Omnis/Cinematics` |
| Localization | Final language resources plus linguistic, truncation, terminology and functional QA | `Config/Localization` and release content |

## Platform evidence

PS5, Xbox Series X/S and Nintendo Switch 2 require authorized SDK/toolchain access, dedicated runners or development hardware, real executable profiling, and preserved evidence. Platform-provider integration must use the actual first-party SDK/API and credentials available to the production team.

## Release and certification

Store submission, production signing, and TRC/TCR certification remain external release operations. The repository contains fail-closed gates so these items cannot be represented as complete before the corresponding evidence exists.
