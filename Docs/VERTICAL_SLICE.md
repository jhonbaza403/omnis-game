# Omnis — First Real Visual Vertical Slice

The repository now contains a deterministic runtime visual slice rather than a static placeholder layout.

## Runtime

When AOmnisGameMode starts, UOmnisWorldBootstrapSubsystem creates:

- a navigable foundation;
- a four-pillar landmark ring;
- a central animated Nexus core;
- two ambient NPCs;
- four discoverable landmarks;
- colored point lights;
- a deterministic player start;
- an in-game HUD showing cycle, phase, world age, vitality and controls.

The visual core is implemented by AOmnisNexusVisualActor and is animated at runtime.

## Persistent editor content

Tools/Editor/GenerateOmnisVerticalSlice.py creates and saves:
 /Game/Omnis/Maps/Omnis_VerticalSlice.umap

The script must be executed in Unreal Engine 5.6 because .umap and .uasset files are Unreal package files and cannot be validly synthesized as plain text.

## Test levels

Python tests validate repository contracts. The Unreal runner validates actual UHT/UBT compilation. The editor generation script validates the real asset-authoring path.

## Controls

- W/A/S/D — movement
- Shift — sprint
- Mouse — camera
- E — interact
- Q — construct
- Space — jump

The first slice intentionally uses engine-native geometry so it can run without third-party asset packs. It is a real playable foundation, not a fake screenshot or a dummy HTML scene.
