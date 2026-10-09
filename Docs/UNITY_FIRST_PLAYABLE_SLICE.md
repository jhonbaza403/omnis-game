# Omnis — first Unity playable slice

## Scope
A small third-person exploration prototype: WASD movement, jump, sprint, mouse look, a Nexus obelisk activated with E, a local +25 XP reward, a Nexus phase change, and a minimal HUD. A Unity Editor script creates and saves the scene and materials using Unity APIs.

This is a prototype slice, not a claim that the full Omnis game has been migrated. The scene uses temporary primitive geometry. The repository's OBJ files are Git LFS pointers in the raw repository view and must be downloaded and verified before importing.

## Source mapping

| Unreal source | Unity implementation | Status |
|---|---|---|
| `Gameplay/Character/OmnisPlayerCharacter.cpp` | `OmnisPlayerController.cs` | Movement, jump, sprint, camera and interaction prototype |
| `Gameplay/Interaction/OmnisInteractionComponent.cpp` | `OmnisPlayerController.ReadInteraction` | Raycast and E key |
| `Gameplay/Interaction/OmnisInteractiveActor.cpp` | `OmnisInteractable.cs` | Local reward and Nexus transition |
| `Core/Cycle/OmnisCycleSubsystem.cpp` | `OmnisGameState.cs` | Partial local cycle state only |
| Unreal progression/reward subsystem | `OmnisGameState.GrantExperience` | Local XP counter only |
| Unreal camera boom and input bindings | Unity camera pivot and Input System | Prototype equivalent |

Unreal C++ plugins, UAssets, UMaps, Blueprints, animation graphs, materials and multiplayer systems are not automatically portable. Each must be imported from verified source assets or reconstructed as Unity-native equivalents.

## Limitations
- No persistence, account integration, multiplayer authority, combat, construction, inventory, quests or production backend integration.
- XP and phase reset on restart.
- Primitive art and built-in HUD are temporary.
- No successful Unity Editor run or player build is claimed until CI executes and its logs/artifacts are inspected.
- Cloudflare remains a separate service.

## Build and verification
The CI job runs `Omnis.Editor.OmnisBuild.BuildLinux`, which calls `OmnisSceneBootstrap.CreateMainScene` to create and save `Assets/Scenes/OmnisMain.unity` and materials if missing, then builds a Linux player. Configure `UNITY_LICENSE` as a masked/protected CI variable. The runner image must include Unity 6000.3.0f1 and Linux IL2CPP support.

Verify that CI imports and compiles scripts, materializes the scene, builds the player, and that the running player supports movement, jump, interaction, +25 XP and Nexus phase. Validate Cloudflare dry-run separately; deployment needs protected Cloudflare variables and manual approval.
