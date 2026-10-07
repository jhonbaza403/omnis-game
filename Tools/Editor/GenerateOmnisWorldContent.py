"""Omnis UE 5.8 open-world content materializer."""
import math
import unreal

MAP_PATH = "/Game/Omnis/Maps/Omnis_OpenWorld"
PROJECT_ROOT = unreal.Paths.project_dir()

FAMILIES = {
    "Architecture": ["Omnis_House","Omnis_Villa","Omnis_Mansion","Omnis_Apartment","Omnis_Office","Omnis_Hotel","Omnis_Hospital","Omnis_School","Omnis_Bank","Omnis_Police_Station","Omnis_Industrial","Omnis_Nightclub"],
    "Interiors": ["Omnis_LivingRoom","Omnis_Kitchen","Omnis_Bedroom","Omnis_Bathroom","Omnis_Garage","Omnis_OfficeInterior","Omnis_NightclubInterior"],
    "Vehicles": ["Omnis_Sedan","Omnis_SUV","Omnis_Bus","Omnis_Truck","Omnis_Police_Car","Omnis_Ambulance","Omnis_Motorcycle"],
    "Aviation": ["Omnis_Commercial_Jet","Omnis_Private_Jet","Omnis_Helicopter"],
    "Nature": ["Omnis_Tree","Omnis_Palm","Omnis_Bush","Omnis_Rock","Omnis_Grass","Omnis_Mountain_Ridge","Omnis_Terrain_Cliff","Omnis_Beach","Omnis_Water_Surface"],
    "Animals": ["Omnis_Dog","Omnis_Cat","Omnis_Horse","Omnis_Bird","Omnis_Fish"],
    "Infrastructure": ["Omnis_Road_Segment","Omnis_Sidewalk","Omnis_Street_Lamp","Omnis_Fence","Omnis_Bridge","Omnis_Airport_Terminal","Omnis_Hangar","Omnis_Runway_Segment","Omnis_Dock","Omnis_Boat"],
    "Tools": ["Omnis_Hammer","Omnis_Wrench","Omnis_Flashlight","Omnis_Toolbox"],
    "Characters": ["Omnis_Wanderer","Omnis_Civilian","Omnis_PoliceOfficer","Omnis_Medic","Omnis_Pilot"]
}

def log(message):
    unreal.log("[OMNIS CONTENT] " + message)

def source_file(family, name):
    return PROJECT_ROOT + "Content/Omnis/SourceAssets/" + family + "/" + name + ".obj"

def asset_path(family, name):
    return "/Game/Omnis/Art/%s/%s.%s" % (family, name, name)

def ensure_dirs():
    for family in FAMILIES:
        unreal.EditorAssetLibrary.make_directory("/Game/Omnis/Art/" + family)
    unreal.EditorAssetLibrary.make_directory("/Game/Omnis/Maps")

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

def spawn_mesh(subsystem, mesh_asset, location, scale=(1,1,1), label="", yaw=0):
    actor = subsystem.spawn_actor_from_class(
        unreal.StaticMeshActor,
        unreal.Vector(*location),
        unreal.Rotator(0, yaw, 0)
    )
    if not actor:
        raise RuntimeError("Could not spawn StaticMeshActor")
    actor.static_mesh_component.set_static_mesh(mesh_asset)
    actor.set_actor_scale3d(unreal.Vector(*scale))
    actor.static_mesh_component.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    if label:
        actor.set_actor_label(label)
    return actor

def populate(subsystem):
    architecture = {n: require_mesh("Architecture", n) for n in FAMILIES["Architecture"]}
    interiors = {n: require_mesh("Interiors", n) for n in FAMILIES["Interiors"]}
    nature = {n: require_mesh("Nature", n) for n in FAMILIES["Nature"]}
    infrastructure = {n: require_mesh("Infrastructure", n) for n in FAMILIES["Infrastructure"]}
    vehicles = {n: require_mesh("Vehicles", n) for n in FAMILIES["Vehicles"]}
    aviation = {n: require_mesh("Aviation", n) for n in FAMILIES["Aviation"]}
    animals = {n: require_mesh("Animals", n) for n in FAMILIES["Animals"]}
    characters = {n: require_mesh("Characters", n) for n in FAMILIES["Characters"]}

    buildings = [
        ("Omnis_House",-3500,-2200),("Omnis_Villa",-1400,-2200),("Omnis_Mansion",800,-2200),
        ("Omnis_Apartment",3000,-2200),("Omnis_Office",-3500,0),("Omnis_Hotel",3000,0),
        ("Omnis_Hospital",-3500,2200),("Omnis_School",-1400,2200),("Omnis_Bank",800,2200),
        ("Omnis_Nightclub",3000,2200),("Omnis_Police_Station",-5200,3600),("Omnis_Industrial",5000,-3600)
    ]
    for name,x,y in buildings:
        spawn_mesh(subsystem, architecture[name], (x,y,0), label=name)

    interior_places=[
        ("Omnis_LivingRoom",-5500,-2200),("Omnis_Kitchen",-4300,-2200),
        ("Omnis_Bedroom",-3100,-2200),("Omnis_Bathroom",-1900,-2200),
        ("Omnis_Garage",-700,-2200),("Omnis_OfficeInterior",500,-2200),
        ("Omnis_NightclubInterior",1800,-2200)
    ]
    for name,x,y in interior_places:
        spawn_mesh(subsystem, interiors[name], (x,y,0), label=name)

    road = infrastructure["Omnis_Road_Segment"]
    sidewalk = infrastructure["Omnis_Sidewalk"]
    lamp = infrastructure["Omnis_Street_Lamp"]
    for index in range(-5,6):
        spawn_mesh(subsystem,road,(index*2000,0,0),label="Road_X_%02d"%index)
        spawn_mesh(subsystem,road,(0,index*2000,0),label="Road_Y_%02d"%index)
    for index in range(-10,11,2):
        spawn_mesh(subsystem,sidewalk,(index*500,-500,0),label="Sidewalk_%02d"%index)
        spawn_mesh(subsystem,lamp,(index*500,-620,0),label="Lamp_%02d"%index)

    spawn_mesh(subsystem,infrastructure["Omnis_Bridge"],(0,5200,0),label="Main_Bridge")
    spawn_mesh(subsystem,infrastructure["Omnis_Dock"],(5200,5200,0),label="Harbor_Dock")
    spawn_mesh(subsystem,infrastructure["Omnis_Boat"],(5200,5200,160),label="Harbor_Boat")
    spawn_mesh(subsystem,infrastructure["Omnis_Airport_Terminal"],(0,9000,0),label="Airport_Terminal")
    spawn_mesh(subsystem,infrastructure["Omnis_Hangar"],(5000,9000,0),label="Airport_Hangar")
    spawn_mesh(subsystem,infrastructure["Omnis_Runway_Segment"],(0,11000,0),(2.5,1,1),"Airport_Runway")

    spawn_mesh(subsystem,nature["Omnis_Mountain_Ridge"],(0,17000,0),(2,1.5,1),"Mountain_Ridge")
    spawn_mesh(subsystem,nature["Omnis_Terrain_Cliff"],(-8000,9000,0),(2,2,1.5),"Terrain_Cliff")
    spawn_mesh(subsystem,nature["Omnis_Beach"],(8000,-12000,0),(2.5,1.5,1),"Coast_Beach")
    spawn_mesh(subsystem,nature["Omnis_Water_Surface"],(8000,-15000,-20),(1.5,1,1),"Ocean_Surface")

    for index in range(48):
        angle=math.radians(index*7.5)
        x=math.cos(angle)*6500.0
        y=math.sin(angle)*6500.0
        spawn_mesh(subsystem,nature["Omnis_Tree"],(x+180,y+80,0),(2,2,2),"Tree_%03d"%index)
        spawn_mesh(subsystem,nature["Omnis_Bush"],(x-180,y-100,0),(1.2,1.2,1.2),"Bush_%03d"%index)
        spawn_mesh(subsystem,nature["Omnis_Rock"],(x+300,y+250,0),label="Rock_%03d"%index)
        spawn_mesh(subsystem,nature["Omnis_Grass"],(x-300,y+180,0),label="Grass_%03d"%index)
        if index%3==0:
            spawn_mesh(subsystem,nature["Omnis_Palm"],(x+420,y-200,0),(1.4,1.4,1.4),"Palm_%03d"%index)

    vehicle_places=[("Omnis_SUV",-800,-500),("Omnis_Bus",100,-500),("Omnis_Truck",1000,-500),
                    ("Omnis_Police_Car",-5000,3300),("Omnis_Ambulance",2500,-2200),
                    ("Omnis_Motorcycle",3500,-500),("Omnis_Sedan",4200,-500)]
    for name,x,y in vehicle_places:
        spawn_mesh(subsystem,vehicles[name],(x,y,1),label=name+"_Display")

    aircraft_places=[("Omnis_Commercial_Jet",0,11000),("Omnis_Private_Jet",3500,11000),("Omnis_Helicopter",6000,11000)]
    for i,(name,x,y) in enumerate(aircraft_places):
        spawn_mesh(subsystem,aviation[name],(x,y,1300+i*120),label=name+"_Display")

    animal_places=[("Omnis_Dog",-4500,-4800),("Omnis_Cat",-3000,-4800),("Omnis_Horse",4800,4800),
                   ("Omnis_Bird",5200,4200),("Omnis_Fish",5600,5000)]
    for i,(name,x,y) in enumerate(animal_places):
        z = -10 if name == "Omnis_Fish" else 1
        spawn_mesh(subsystem,animals[name],(x,y,z),label=name+"_Ambient_%02d"%i)

    character_places=[("Omnis_Civilian",-4200,1000),("Omnis_PoliceOfficer",-4700,3600),
                      ("Omnis_Medic",-3300,2500),("Omnis_Pilot",0,8500),("Omnis_Wanderer",420,430)]
    for name,x,y in character_places:
        spawn_mesh(subsystem,characters[name],(x,y,1),label=name+"_SourcePreview")

def add_lighting(subsystem):
    sun=subsystem.spawn_actor_from_class(unreal.DirectionalLight,unreal.Vector(0,0,1500),unreal.Rotator(-45,-30,0))
    sun.set_actor_label("Omnis_Sun")
    sun.get_light_component().set_editor_property("intensity",5.0)

    sky=subsystem.spawn_actor_from_class(unreal.SkyLight,unreal.Vector(0,0,800),unreal.Rotator(0,0,0))
    sky.set_actor_label("Omnis_SkyLight")
    sky.get_light_component().set_editor_property("real_time_capture",True)

    fog=subsystem.spawn_actor_from_class(unreal.ExponentialHeightFog,unreal.Vector(0,0,0),unreal.Rotator(0,0,0))
    fog.set_actor_label("Omnis_AtmosphereFog")

def main():
    ensure_dirs()
    import_all()
    subsystem=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    if not levels.new_level(MAP_PATH):
        raise RuntimeError("Could not create level: " + MAP_PATH)

    populate(subsystem)
    add_lighting(subsystem)

    if not levels.save_current_level():
        raise RuntimeError("Could not save level: " + MAP_PATH)

    log("Persistent map created: " + MAP_PATH)

if __name__ == "__main__":
    main()
