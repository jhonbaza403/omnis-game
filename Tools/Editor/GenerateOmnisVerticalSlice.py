"""
Unreal Engine 5.6 Editor script for Omnis' first persistent visual vertical slice.

Run inside Unreal Editor:
    Python -> Execute Python Script

Creates and saves:
    /Game/Omnis/Maps/Omnis_VerticalSlice.umap

The saved map is a real Unreal package. Runtime gameplay and the animated
Nexus visual are supplied by the Omnis C++ modules.
"""

import unreal

MAP_PATH = "/Game/Omnis/Maps/Omnis_VerticalSlice"


def log(message):
    unreal.log("[OMNIS CONTENT] " + message)


def spawn(actor_subsystem, actor_class, location, rotation=unreal.Rotator(0, 0, 0)):
    actor = actor_subsystem.spawn_actor_from_class(
        actor_class,
        unreal.Vector(*location),
        rotation
    )
    if not actor:
        raise RuntimeError("Could not spawn " + actor_class.__name__)
    return actor


def main():
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)\n    unreal.EditorAssetLibrary.make_directory("/Game/Omnis")
    unreal.EditorAssetLibrary.make_directory("/Game/Omnis/Maps")

    if not level_subsystem.new_level(MAP_PATH):
        raise RuntimeError("Could not create level " + MAP_PATH)

    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("Editor world was not created")

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    source_files = [
        (
            unreal.Paths.project_dir() + "Content/Omnis/SourceAssets/Obelisk/Omnis_Obelisk.obj",
            "/Game/Omnis/Art/Obelisk"
        ),
        (
            unreal.Paths.project_dir() + "Content/Omnis/SourceAssets/Textures/Omnis_Nexus_Albedo.png",
            "/Game/Omnis/Art/Textures"
        ),
    ]

    unreal.EditorAssetLibrary.make_directory("/Game/Omnis/Art")
    unreal.EditorAssetLibrary.make_directory("/Game/Omnis/Art/Textures")

    import_tasks = []
    for filename, destination in source_files:
        task = unreal.AssetImportTask()
        task.set_editor_property("filename", filename)
        task.set_editor_property("destination_path", destination)
        task.set_editor_property("automated", True)
        task.set_editor_property("replace_existing", True)
        task.set_editor_property("save", True)
        import_tasks.append(task)

    asset_tools.import_asset_tasks(import_tasks)

    imported_mesh_assets = unreal.EditorAssetLibrary.list_assets(
        "/Game/Omnis/Art/Obelisk",
        recursive=True,
        include_folder=False
    )
    imported_texture_assets = unreal.EditorAssetLibrary.list_assets(
        "/Game/Omnis/Art/Textures",
        recursive=True,
        include_folder=False
    )

    if not imported_mesh_assets:
        raise RuntimeError("The authored Obelisk OBJ did not import into Unreal")
    if not imported_texture_assets:
        raise RuntimeError("The authored Nexus texture did not import into Unreal")

    obelisk_mesh = None
    for asset_path in imported_mesh_assets:
        candidate = unreal.EditorAssetLibrary.load_asset(asset_path)
        if isinstance(candidate, unreal.StaticMesh):
            obelisk_mesh = candidate
            break

    nexus_texture = unreal.EditorAssetLibrary.load_asset(imported_texture_assets[0])
    if not obelisk_mesh:
        raise RuntimeError("The authored Obelisk did not produce a StaticMesh asset")
    if not nexus_texture:
        raise RuntimeError("The authored Nexus texture could not be loaded")

    cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    sphere = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Sphere.Sphere")
    if not cube or not sphere:
        raise RuntimeError("Required engine BasicShapes assets are unavailable")

    player_start = spawn(actor_subsystem, unreal.PlayerStart, (0, 0, 125))
    player_start.set_actor_label("Omnis_PlayerStart")

    foundation = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 0, -50))
    foundation.set_actor_label("Omnis_Foundation")
    foundation.static_mesh_component.set_static_mesh(cube)
    foundation.set_actor_scale3d(unreal.Vector(30, 30, 0.5))

    nexus = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 0, 80))
    nexus.set_actor_label("Omnis_Nexus_EditorPreview")
    nexus.static_mesh_component.set_static_mesh(sphere)
    nexus.set_actor_scale3d(unreal.Vector(2.2, 2.2, 2.2))

    obelisk = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 950, 95))
    obelisk.set_actor_label("Omnis_Authored_Obelisk")
    obelisk.static_mesh_component.set_static_mesh(obelisk_mesh)
    obelisk.set_actor_scale3d(unreal.Vector(140, 140, 140))

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

    if not unreal.EditorLevelLibrary.save_current_level():
        raise RuntimeError("Could not save current level")

    log("Persistent map created: " + MAP_PATH)
    log("Open the map and press Play to execute the real Omnis C++ vertical slice.")


if __name__ == "__main__":
    main()
