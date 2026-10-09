# Omnis Unity packaging and release acceptance

## Supported Unity editor

The Unity slice is pinned to Unity 6000.3.0f1 and URP 17.3.0. The editor setup script generates the URP pipeline and Forward+ renderer assets and applies a 3840 x 2160 standalone default when the project opens or a build method runs.

## Build from the Unity Editor

Open Unity Hub and add the repository folder UnityProject. Open Assets/Scenes/OmnisMain.unity and press Play. The scene bootstraps a deterministic playable vertical slice: exploration, three relic objectives, Nexus activation, local sentinel AI, melee pulse combat, XP, and a cycle restart.

Use the Unity menu Omnis > Build > Build Windows 64-bit to produce UnityProject/Builds/Windows/Omnis.exe, or Omnis > Build > Build Linux 64-bit for UnityProject/Builds/Linux/Omnis.x86_64. The OmnisBuild methods fail with an exception if Unity's BuildPipeline reports a non-success result and write a release manifest only after a successful build.

## Native Windows CI

Windows build CI is manual and uses the runner tag omnis-windows. Register a protected project runner on a Windows machine with Unity 6000.3.0f1 and the Windows Build Support module installed. Configure the protected CI/CD variable UNITY_LICENSE as a File variable containing a valid Unity license file. UNITY_EDITOR_PATH is optional; by default the job uses the standard Unity Hub install path.

## Acceptance gate

A deliverable is not release-ready solely because code or a build script exists. Release approval requires:
1. The Unity editor validation job completes.
2. A platform build finishes successfully and publishes an artifact directory plus omnis-release-manifest.json.
3. Launch the built player outside the editor.
4. Test keyboard/mouse input, all three relics, one-time rewards, Nexus gate, sentinel combat, death/respawn, saved XP/cycle state, and cycle reset.
5. Verify 3840 x 2160 output on the target display and capture frame time / memory on the target GPU. Reduce quality settings if the target cannot sustain its performance goal.

The current world uses deterministic procedural geometry and a generated 512 px albedo noise texture to keep the slice self-contained. It is a stylized visual baseline, not a claim of AAA photorealistic art or a substitute for a fully authored asset pipeline.
