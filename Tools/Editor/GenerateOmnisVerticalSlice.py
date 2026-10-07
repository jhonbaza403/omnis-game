"""
Unreal Engine 5.6 Editor script for Omnis' first persistent visual vertical slice.

Run inside Unreal Editor:
    Python -> Execute Python Script

Creates and saves:
    /Game/Omnis/Maps/Omnis_VerticalSlice.umap

The level uses real Unreal engine-authored meshes/lights and the Omnis
runtime bootstrap. It does not fabricate binary .uasset files.
"""

import unreal

MAP_PATH = "/Game/Omnis/Maps/Omnis_VerticalSlice"


def log(message):
    unreal.log("[OMNIS CONTENT] " + message)


def spawn(actor_subsystem, actor_class, location, rotation=unreal.Rotator(0, 0, 0)):
    actor = actor_subsystem.spawn_actor_from_class(actor_class, unreal.Vector(*location), rotation)
    if not actor:
        raise RuntimeError("Could not spawn " + actor_class.__name__)
    return actor


def main():
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    if not level_subsystem.new_level(MAP_PATH):
        raise RuntimeError("Could not create level " + MAP_PATH)

    cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    sphere = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Sphere.Sphere")
    if not cube or not sphere:
        raise RuntimeError("Required engine BasicShapes assets are unavailable")

    player_start = spawn(actor_subsystem, unreal.PlayerStart, (0, 0, 120))
    player_start.set_actor_label("Omnis_PlayerStart")

    foundation = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 0, -50))
    foundation.set_actor_label("Omnis_Foundation")
    foundation.static_mesh_component.set_static_mesh(cube)
    foundation.set_actor_scale3d(unreal.Vector(30, 30, 0.5))

    core = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 0, 100))
    core.set_actor_label("Omnis_NexusCore")
    core.static_mesh_component.set_static_mesh(sphere)
    core.set_actor_scale3d(unreal.Vector(2.5, 2.5, 2.5))

    for index, (x, y) in enumerate(((800, 0), (-800, 0), (0, 800), (0, -800)), 1):
        pillar = spawn(actor_subsystem, unreal.StaticMeshActor, (x, y, 120))
        pillar.set_actor_label("Omnis_Pillar_%02d" % index)
        pillar.static_mesh_component.set_static_mesh(cube)
        pillar.set_actor_scale3d(unreal.Vector(2, 2, 3))

    directional = spawn(
        actor_subsystem,
        unreal.DirectionalLight,
        (0, 0, 800),
        unreal.Rotator(-45, -30, 0)
    )
    directional.get_light_component().set_editor_property("intensity", 5.0)

    sky = spawn(actor_subsystem, unreal.SkyLight, (0, 0, 500))
    sky.get_light_component().set_editor_property("real_time_capture", True)

    fog = spawn(actor_subsystem, unreal.ExponentialHeightFog, (0, 0, 0))
    fog_component = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    if fog_component:
        fog_component.set_editor_property("fog_density", 0.015)

    if not unreal.EditorAssetLibrary.save_asset(MAP_PATH):
        raise RuntimeError("Could not save " + MAP_PATH)

    log("Persistent map created: " + MAP_PATH)
    log("Press Play on this map to execute the Omnis C++ world bootstrap.")


if __name__ == "__main__":
    main()
