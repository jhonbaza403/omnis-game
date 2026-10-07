"""
Omnis UE 5.6 editor content generator.

Run inside Unreal Editor with PythonScriptPlugin enabled.

Creates:
  /Game/Omnis/Maps/Omnis_VerticalSlice
  /Game/Omnis/Art/Characters/*
  /Game/Omnis/Art/Obelisk/*
  /Game/Omnis/Art/Textures/*
  /Game/Omnis/Art/Materials/*
"""
import unreal

MAP_PATH = "/Game/Omnis/Maps/Omnis_VerticalSlice"
PROJECT_ROOT = unreal.Paths.project_dir()


def log(message):
    unreal.log("[OMNIS CONTENT] " + message)


def spawn(actor_subsystem, actor_class, location, rotation=unreal.Rotator(0, 0, 0)):
    actor = actor_subsystem.spawn_actor_from_class(
        actor_class,
        unreal.Vector(*location),
        rotation,
    )
    if not actor:
        raise RuntimeError("Could not spawn " + str(actor_class))
    return actor


def ensure_directories():
    for path in (
        "/Game/Omnis",
        "/Game/Omnis/Maps",
        "/Game/Omnis/Art",
        "/Game/Omnis/Art/Characters",
        "/Game/Omnis/Art/Obelisk",
        "/Game/Omnis/Art/Textures",
        "/Game/Omnis/Art/Materials",
    ):
        unreal.EditorAssetLibrary.make_directory(path)


def import_source_assets(asset_tools):
    source_files = [
        (
            PROJECT_ROOT + "Content/Omnis/SourceAssets/Obelisk/Omnis_Obelisk.obj",
            "/Game/Omnis/Art/Obelisk",
        ),
        (
            PROJECT_ROOT + "Content/Omnis/SourceAssets/Characters/Omnis_Wanderer.obj",
            "/Game/Omnis/Art/Characters",
        ),
        (
            PROJECT_ROOT + "Content/Omnis/SourceAssets/Textures/Omnis_Nexus_Albedo.png",
            "/Game/Omnis/Art/Textures",
        ),
    ]

    tasks = []
    for filename, destination in source_files:
        if not unreal.Paths.file_exists(filename):
            raise RuntimeError("Source asset missing: " + filename)
        task = unreal.AssetImportTask()
        task.set_editor_property("filename", filename)
        task.set_editor_property("destination_path", destination)
        task.set_editor_property("automated", True)
        task.set_editor_property("replace_existing", True)
        task.set_editor_property("save", True)
        tasks.append(task)

    asset_tools.import_asset_tasks(tasks)

    imported = {}
    for destination in (
        "/Game/Omnis/Art/Obelisk",
        "/Game/Omnis/Art/Characters",
        "/Game/Omnis/Art/Textures",
    ):
        assets = unreal.EditorAssetLibrary.list_assets(
            destination, recursive=True, include_folder=False
        )
        if not assets:
            raise RuntimeError("No assets imported under " + destination)
        imported[destination] = assets

    return imported


def create_material(asset_tools, name, color, texture=None):
    package_path = "/Game/Omnis/Art/Materials"
    asset_path = package_path + "/" + name
    existing = unreal.EditorAssetLibrary.load_asset(asset_path)
    if existing:
        return existing

    factory = unreal.MaterialFactoryNew()
    material = asset_tools.create_asset(name, package_path, unreal.Material, factory)
    if not material:
        raise RuntimeError("Could not create material " + name)

    editing = unreal.MaterialEditingLibrary

    base = unreal.MaterialExpressionVectorParameter()
    base.parameter_name = "BaseColor"
    base.default_value = unreal.LinearColor(*color, 1.0)
    editing.connect_material_property(base, "", unreal.MaterialProperty.MP_BASE_COLOR)
    editing.connect_material_property(base, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    roughness = unreal.MaterialExpressionScalarParameter()
    roughness.parameter_name = "Roughness"
    roughness.default_value = 0.35
    editing.connect_material_property(roughness, "", unreal.MaterialProperty.MP_ROUGHNESS)

    if texture:
        tex = unreal.MaterialExpressionTextureSample()
        tex.texture = texture
        editing.connect_material_property(tex, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)

    editing.layout_material_expressions(material)
    editing.recompile_material(material)
    unreal.EditorAssetLibrary.save_asset(asset_path)
    return material


def find_static_mesh(assets):
    for asset_path in assets:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if isinstance(asset, unreal.StaticMesh):
            return asset
    return None


def main():
    ensure_directories()

    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    imported = import_source_assets(asset_tools)

    character_mesh = find_static_mesh(imported["/Game/Omnis/Art/Characters"])
    obelisk_mesh = find_static_mesh(imported["/Game/Omnis/Art/Obelisk"])
    texture_assets = imported["/Game/Omnis/Art/Textures"]
    texture = unreal.EditorAssetLibrary.load_asset(texture_assets[0])

    if not character_mesh:
        raise RuntimeError("The authored character did not import as a StaticMesh")
    if not obelisk_mesh:
        raise RuntimeError("The authored obelisk did not import as a StaticMesh")
    if not texture:
        raise RuntimeError("The authored Nexus texture could not be loaded")

    create_material(asset_tools, "M_Omnis_Nexus", (0.18, 0.55, 0.95), texture)
    create_material(asset_tools, "M_Omnis_Wanderer", (0.20, 0.32, 0.48))
    create_material(asset_tools, "M_Omnis_Obelisk", (0.10, 0.28, 0.42))

    if not level_subsystem.new_level(MAP_PATH):
        raise RuntimeError("Could not create level " + MAP_PATH)

    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("Editor world was not created")

    cube = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cube.Cube")
    sphere = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Sphere.Sphere")
    cylinder = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cylinder.Cylinder")
    if not cube or not sphere or not cylinder:
        raise RuntimeError("Required UE 5.6 BasicShapes assets are unavailable")

    player_start = spawn(actor_subsystem, unreal.PlayerStart, (-900, 0, 125))
    player_start.set_actor_label("Omnis_PlayerStart")

    foundation = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 0, -50))
    foundation.set_actor_label("Omnis_Foundation")
    foundation.static_mesh_component.set_static_mesh(cube)
    foundation.set_actor_scale3d(unreal.Vector(35, 35, 0.5))

    nexus = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 0, 120))
    nexus.set_actor_label("Omnis_Nexus_EditorPreview")
    nexus.static_mesh_component.set_static_mesh(sphere)
    nexus.set_actor_scale3d(unreal.Vector(2.2, 2.2, 2.2))

    obelisk = spawn(actor_subsystem, unreal.StaticMeshActor, (0, 950, 120))
    obelisk.set_actor_label("Omnis_Authored_Obelisk")
    obelisk.static_mesh_component.set_static_mesh(obelisk_mesh)
    obelisk.set_actor_scale3d(unreal.Vector(130, 130, 130))

    wanderer = spawn(actor_subsystem, unreal.StaticMeshActor, (420, 430, 120))
    wanderer.set_actor_label("Omnis_Authored_Wanderer")
    wanderer.static_mesh_component.set_static_mesh(character_mesh)
    wanderer.set_actor_scale3d(unreal.Vector(75, 75, 75))

    for index, (x, y) in enumerate(((800, 0), (-800, 0), (0, 800), (0, -800)), 1):
        pillar = spawn(actor_subsystem, unreal.StaticMeshActor, (x, y, 120))
        pillar.set_actor_label("Omnis_Pillar_%02d" % index)
        pillar.static_mesh_component.set_static_mesh(cylinder)
        pillar.set_actor_scale3d(unreal.Vector(2, 2, 3))

    directional = spawn(
        actor_subsystem,
        unreal.DirectionalLight,
        (0, 0, 800),
        unreal.Rotator(-45, -30, 0),
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
    log("Imported authored character, obelisk, texture and materials.")
    log("Run the C++ game to activate runtime systems and the animated Nexus.")


if __name__ == "__main__":
    main()
