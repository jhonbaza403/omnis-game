"""Omnis UE 5.6 open-world content materializer.

Imports authored source meshes into native Unreal Static Mesh assets and creates
a persistent content map with urban, residential, public-service, nightlife,
airport, harbor, interior and nature zones.

This script must be executed by a real Unreal Engine 5.6 Editor instance.
It intentionally fails rather than inventing missing native assets.
"""
import math
import unreal

MAP_PATH = "/Game/Omnis/Maps/Omnis_OpenWorld"
PROJECT_ROOT = unreal.Paths.project_dir()

FAMILIES = {
    "Architecture": [
        "Omnis_House", "Omnis_Villa", "Omnis_Mansion", "Omnis_Apartment",
        "Omnis_Office", "Omnis_Hotel", "Omnis_Hospital", "Omnis_School",
        "Omnis_Bank", "Omnis_Police_Station", "Omnis_Industrial",
        "Omnis_Nightclub"
    ],
    "Interiors": [
        "Omnis_LivingRoom", "Omnis_Kitchen", "Omnis_Bedroom",
        "Omnis_Bathroom", "Omnis_Garage", "Omnis_OfficeInterior",
        "Omnis_NightclubInterior"
    ],
    "Vehicles": [
        "Omnis_Sedan", "Omnis_SUV", "Omnis_Bus", "Omnis_Truck",
        "Omnis_Police_Car", "Omnis_Ambulance", "Omnis_Motorcycle"
    ],
    "Aviation": [
        "Omnis_Commercial_Jet", "Omnis_Private_Jet", "Omnis_Helicopter"
    ],
    "Nature": [
        "Omnis_Tree", "Omnis_Palm", "Omnis_Bush", "Omnis_Rock", "Omnis_Grass"
    ],
    "Animals": [
        "Omnis_Dog", "Omnis_Cat", "Omnis_Horse", "Omnis_Bird", "Omnis_Fish"
    ],
    "Infrastructure": [
        "Omnis_Road_Segment", "Omnis_Sidewalk", "Omnis_Street_Lamp",
        "Omnis_Fence", "Omnis_Bridge", "Omnis_Airport_Terminal",
        "Omnis_Hangar", "Omnis_Runway_Segment", "Omnis_Dock", "Omnis_Boat"
    ],
    "Tools": [
        "Omnis_Hammer", "Omnis_Wrench", "Omnis_Flashlight", "Omnis_Toolbox"
    ],
    "Characters": [
        "Omnis_Wanderer", "Omnis_Civilian", "Omnis_PoliceOfficer",
        "Omnis_Medic", "Omnis_Pilot"
    ]
}

def log(message):
    unreal.log("[OMNIS UE5.6] " + message)

def source_file(family, name):
    return PROJECT_ROOT + "Content/Omnis/SourceAssets/" + family + "/" + name + ".obj"

def asset_path(family, name):
    return "/Game/Omnis/Art/%s/%s.%s" % (family, name, name)

def ensure_directories():
    directories = ["/Game/Omnis", "/Game/Omnis/Maps", "/Game/Omnis/Art"]
    for family in FAMILIES:
        directories.append("/Game/Omnis/Art/" + family)
    for path in directories:
        unreal.EditorAssetLibrary.make_directory(path)

def import_all():
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    tasks = []

    for family, names in FAMILIES.items():
        for name in names:
            filename = source_file(family, name)
            if not unreal.Paths.file_exists(filename):
                raise RuntimeError("Missing authored source mesh: " + filename)

            task = unreal.AssetImportTask()
            task.set_editor_property("filename", filename)
            task.set_editor_property("destination_path", "/Game/Omnis/Art/" + family)
            task.set_editor_property("automated", True)
            task.set_editor_property("replace_existing", True)
            task.set_editor_property("save", True)
            tasks.append(task)

    tools.import_asset_tasks(tasks)

def require_mesh(family, name):
    path = asset_path(family, name)
    mesh = unreal.EditorAssetLibrary.load_asset(path)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        raise RuntimeError("Native StaticMesh missing after import: " + path)
    return mesh

def spawn_mesh(actor_subsystem, mesh_asset, location, scale=(1, 1, 1), label=""):
    actor = actor_subsystem.spawn_actor_from_class(
        unreal.StaticMeshActor,
        unreal.Vector(*location),
        unreal.Rotator(0, 0, 0),
    )
    if not actor:
        raise RuntimeError("Could not spawn StaticMeshActor")

    actor.static_mesh_component.set_static_mesh(mesh_asset)
    actor.set_actor_scale3d(unreal.Vector(*scale))

    if label:
        actor.set_actor_label(label)

    return actor

def populate(actor_subsystem):
    architecture = {name: require_mesh("Architecture", name) for name in FAMILIES["Architecture"]}
    interiors = {name: require_mesh("Interiors", name) for name in FAMILIES["Interiors"]}
    nature = {name: require_mesh("Nature", name) for name in FAMILIES["Nature"]}
    infrastructure = {name: require_mesh("Infrastructure", name) for name in FAMILIES["Infrastructure"]}
    vehicles = {name: require_mesh("Vehicles", name) for name in FAMILIES["Vehicles"]}
    aviation = {name: require_mesh("Aviation", name) for name in FAMILIES["Aviation"]}
    animals = {name: require_mesh("Animals", name) for name in FAMILIES["Animals"]}
    characters = {name: require_mesh("Characters", name) for name in FAMILIES["Characters"]}

    placements = [
        ("Omnis_House", -3500, -2200),
        ("Omnis_Villa", -1400, -2200),
        ("Omnis_Mansion", 800, -2200),
        ("Omnis_Apartment", 3000, -2200),
        ("Omnis_Office", -3500, 0),
        ("Omnis_Hotel", 3000, 0),
        ("Omnis_Hospital", -3500, 2200),
        ("Omnis_School", -1400, 2200),
        ("Omnis_Bank", 800, 2200),
        ("Omnis_Nightclub", 3000, 2200),
        ("Omnis_Police_Station", -5200, 3600),
        ("Omnis_Industrial", 5000, -3600)
    ]
    for name, x, y in placements:
        spawn_mesh(actor_subsystem, architecture[name], (x, y, 0), label=name)

    # Interior demonstrators are placed as separate accessible interiors until
    # full architectural shells are authored and opened for walk-through.
    interior_places = [
        ("Omnis_LivingRoom", -5500, -2200),
        ("Omnis_Kitchen", -4300, -2200),
        ("Omnis_Bedroom", -3100, -2200),
        ("Omnis_Bathroom", -1900, -2200),
        ("Omnis_Garage", -700, -2200),
        ("Omnis_OfficeInterior", 500, -2200),
        ("Omnis_NightclubInterior", 1800, -2200)
    ]
    for name, x, y in interior_places:
        spawn_mesh(actor_subsystem, interiors[name], (x, y, 0), label=name)

    road = infrastructure["Omnis_Road_Segment"]
    sidewalk = infrastructure["Omnis_Sidewalk"]
    lamp = infrastructure["Omnis_Street_Lamp"]

    for index in range(-5, 6):
        spawn_mesh(actor_subsystem, road, (index * 2000, 0, -10), label="Road_X_%02d" % index)
        spawn_mesh(actor_subsystem, road, (0, index * 2000, -10), label="Road_Y_%02d" % index)

    for index in range(-10, 11, 2):
        spawn_mesh(actor_subsystem, sidewalk, (index * 500, -500, 40), label="Sidewalk_%02d" % index)
        spawn_mesh(actor_subsystem, lamp, (index * 500, -620, 0), label="Lamp_%02d" % index)

    spawn_mesh(actor_subsystem, infrastructure["Omnis_Bridge"], (0, 5200, 0), label="Main_Bridge")
    spawn_mesh(actor_subsystem, infrastructure["Omnis_Dock"], (5200, 5200, 0), label="Harbor_Dock")
    spawn_mesh(actor_subsystem, infrastructure["Omnis_Boat"], (5200, 5200, 260), label="Harbor_Boat")

    spawn_mesh(actor_subsystem, infrastructure["Omnis_Airport_Terminal"], (0, 9000, 0),
               label="Airport_Terminal")
    spawn_mesh(actor_subsystem, infrastructure["Omnis_Hangar"], (5000, 9000, 0),
               label="Airport_Hangar")
    spawn_mesh(actor_subsystem, infrastructure["Omnis_Runway_Segment"], (0, 11000, 0),
               scale=(2.5, 1, 1), label="Airport_Runway")

    for index in range(48):
        angle = math.radians(index * 7.5)
        x = math.cos(angle) * 6500.0
        y = math.sin(angle) * 6500.0
        spawn_mesh(actor_subsystem, nature["Omnis_Tree"], (x + 180, y + 80, 0),
                   scale=(2, 2, 2), label="Tree_%03d" % index)
        spawn_mesh(actor_subsystem, nature["Omnis_Bush"], (x - 180, y - 100, 0),
                   scale=(1.2, 1.2, 1.2), label="Bush_%03d" % index)
        spawn_mesh(actor_subsystem, nature["Omnis_Rock"], (x + 300, y + 250, 0),
                   label="Rock_%03d" % index)
        if index % 3 == 0:
            spawn_mesh(actor_subsystem, nature["Omnis_Palm"], (x + 420, y - 200, 0),
                       scale=(1.4, 1.4, 1.4), label="Palm_%03d" % index)

    # Place static displays for all vehicle and aircraft families so the editor
    # map contains every authored transport family.
    vehicle_places = [
        ("Omnis_SUV", -800, -500), ("Omnis_Bus", 100, -500),
        ("Omnis_Truck", 1000, -500), ("Omnis_Police_Car", -5000, 3300),
        ("Omnis_Ambulance", 2500, -2200), ("Omnis_Motorcycle", 3500, -500),
        ("Omnis_Sedan", 4200, -500)
    ]
    for name, x, y in vehicle_places:
        spawn_mesh(actor_subsystem, vehicles[name], (x, y, 170), label=name + "_Display")

    aircraft_places = [
        ("Omnis_Commercial_Jet", 0, 11000),
        ("Omnis_Private_Jet", 3500, 11000),
        ("Omnis_Helicopter", 6000, 11000)
    ]
    for index, (name, x, y) in enumerate(aircraft_places):
        spawn_mesh(actor_subsystem, aviation[name], (x, y, 1300 + index * 120),
                   label=name + "_Display")

    # Wildlife display/ambient population points.
    for index, (name, x, y) in enumerate([
        ("Omnis_Dog", -4500, -4800),
        ("Omnis_Cat", -3000, -4800),
        ("Omnis_Horse", 4800, 4800),
        ("Omnis_Bird", 5200, 4200),
        ("Omnis_Fish", 5600, 5000)
    ]):
        spawn_mesh(actor_subsystem, animals[name], (x, y, 120), label=name + "_Ambient_%02d" % index)

    # Service-character source meshes are materialized for later skeletal
    # retargeting and full-body production animation.
    character_places = [
        ("Omnis_Civilian", -4200, 1000),
        ("Omnis_PoliceOfficer", -4700, 3600),
        ("Omnis_Medic", -3300, 2500),
        ("Omnis_Pilot", 0, 8500),
        ("Omnis_Wanderer", 420, 430)
    ]
    for name, x, y in character_places:
        spawn_mesh(actor_subsystem, characters[name], (x, y, 120),
                   scale=(1, 1, 1), label=name + "_SourcePreview")

def add_lighting(actor_subsystem):
    sun = actor_subsystem.spawn_actor_from_class(
        unreal.DirectionalLight,
        unreal.Vector(0, 0, 1500),
        unreal.Rotator(-45, -30, 0),
    )
    sun.set_actor_label("Omnis_Sun")
    sun.get_light_component().set_editor_property("intensity", 5.0)

    sky = actor_subsystem.spawn_actor_from_class(
        unreal.SkyLight,
        unreal.Vector(0, 0, 800),
        unreal.Rotator(0, 0, 0),
    )
    sky.set_actor_label("Omnis_SkyLight")
    sky.get_light_component().set_editor_property("real_time_capture", True)

    fog = actor_subsystem.spawn_actor_from_class(
        unreal.ExponentialHeightFog,
        unreal.Vector(0, 0, 0),
        unreal.Rotator(0, 0, 0),
    )
    fog.set_actor_label("Omnis_AtmosphereFog")

def main():
    ensure_directories()
    import_all()

    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    if not level_subsystem.new_level(MAP_PATH):
        raise RuntimeError("Could not create level: " + MAP_PATH)

    populate(actor_subsystem)
    add_lighting(actor_subsystem)

    if not level_subsystem.save_current_level():
        raise RuntimeError("Could not save level: " + MAP_PATH)

    log("Materialized native content under /Game/Omnis/Art")
    log("Saved persistent map: " + MAP_PATH)

if __name__ == "__main__":
    main()
