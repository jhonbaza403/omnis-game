"""Premium materialization layer for Omnis UE 5.8."""
import math
import random
import unreal
import GenerateOmnisWorldContent as base

MAP_PATH = base.MAP_PATH
TEMPLATE = "/Engine/Maps/Templates/OpenWorld"
MAT_ROOT = "/Game/Omnis/Materials/Premium"
ART_ROOT = "/Game/Omnis/Art"
ROOT = unreal.Paths.project_dir()

COLORS = {
    "Architecture": ((0.30,0.24,0.18,1),0.78,0.0),
    "Interiors": ((0.20,0.16,0.13,1),0.62,0.0),
    "Infrastructure": ((0.18,0.19,0.18,1),0.86,0.02),
    "Nature": ((0.16,0.24,0.10,1),0.92,0.0),
    "Vehicles": ((0.10,0.14,0.18,1),0.28,0.12),
    "Aviation": ((0.18,0.20,0.22,1),0.25,0.22),
    "Obelisk": ((0.07,0.10,0.13,1),0.34,0.55),
    "Characters": ((0.19,0.19,0.17,1),0.82,0.0),
    "Animals": ((0.32,0.25,0.18,1),0.90,0.0),
}

def log(s):
    unreal.log("[OMNIS PREMIUM] " + s)

def make_mat(name,color,rough,metal):
    path=MAT_ROOT+"/"+name
    m=unreal.EditorAssetLibrary.load_asset(path)
    if m: return m

    unreal.EditorAssetLibrary.make_directory(MAT_ROOT)
    m=unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        name, MAT_ROOT, unreal.Material, unreal.MaterialFactoryNew())
    if not m:
        raise RuntimeError("Could not create material: " + path)

    p=unreal.MaterialEditingLibrary.create_material_expression(
        m, unreal.MaterialExpressionVectorParameter, -360, -80)
    p.set_editor_property("parameter_name",name+"_BaseColor")
    p.set_editor_property("default_value",unreal.LinearColor(*color))

    r=unreal.MaterialEditingLibrary.create_material_expression(
        m, unreal.MaterialExpressionScalarParameter, -360, 80)
    r.set_editor_property("parameter_name",name+"_Roughness")
    r.set_editor_property("default_value",rough)

    k=unreal.MaterialEditingLibrary.create_material_expression(
        m, unreal.MaterialExpressionScalarParameter, -360, 240)
    k.set_editor_property("parameter_name",name+"_Metallic")
    k.set_editor_property("default_value",metal)

    unreal.MaterialEditingLibrary.connect_material_property(
        p,"",unreal.MaterialProperty.MP_BASE_COLOR)
    unreal.MaterialEditingLibrary.connect_material_property(
        r,"",unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.connect_material_property(
        k,"",unreal.MaterialProperty.MP_METALLIC)
    unreal.MaterialEditingLibrary.recompile_material(m)
    unreal.EditorAssetLibrary.save_loaded_asset(m)
    return m

def materials():
    out={f:make_mat("M_"+f+"_Premium",c,r,k) for f,(c,r,k) in COLORS.items()}
    out["Water"]=make_mat(
        "M_Water_Premium",(0.035,0.14,0.20,1),0.16,0.30)
    return out

def import_obelisk():
    filename=ROOT+"Content/Omnis/SourceAssets/Obelisk/Omnis_Obelisk.obj"
    if not unreal.Paths.file_exists(filename):
        raise RuntimeError("Missing authored Obelisk source mesh: " + filename)

    task=unreal.AssetImportTask()
    task.set_editor_property("filename",filename)
    task.set_editor_property("destination_path",ART_ROOT+"/Obelisk")
    task.set_editor_property("automated",True)
    task.set_editor_property("replace_existing",True)
    task.set_editor_property("save",True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

def spawn_premium(sub,mesh,loc,scale,label,mat,yaw=0):
    actor=base.spawn_mesh(sub,mesh,loc,scale,label,yaw)
    actor.static_mesh_component.set_material(0,mat)
    return actor

def extra_dressing(sub,m,mat):
    rng=random.Random(20261007)
    N=m["Nature"]
    I=m["Infrastructure"]
    A=m["Architecture"]

    blocks=[
        ("Omnis_House",(-8600,-5200)),("Omnis_House",(-7600,-5200)),
        ("Omnis_Villa",(-6500,-6200)),("Omnis_House",(-5200,-6200)),
        ("Omnis_Apartment",(5200,-5200)),("Omnis_Office",(6500,-5200)),
        ("Omnis_Hotel",(7600,-4500)),("Omnis_School",(-8200,2200)),
        ("Omnis_House",(-7300,3200)),("Omnis_Bank",(6900,2200)),
        ("Omnis_Nightclub",(7900,2100)),("Omnis_Industrial",(8200,-7200))
    ]

    for i,(name,(x,y)) in enumerate(blocks):
        s=rng.uniform(0.85,1.20)
        spawn_premium(
            sub,A[name],(x,y,20),(s,s,s),
            "Premium_Block_%02d"%i,mat["Architecture"],rng.uniform(0,360))

    for i in range(240):
        ang=math.radians(i*1.5+rng.uniform(-7,7))
        rad=rng.uniform(7000,16000)
        x=math.cos(ang)*rad
        y=math.sin(ang)*rad
        st=rng.uniform(1.0,2.4)
        spawn_premium(
            sub,N["Omnis_Tree"],(x,y,40),(st,st,st),
            "Premium_Tree_%03d"%i,mat["Nature"],rng.uniform(0,360))

        if i%2==0:
            b=rng.uniform(0.65,1.4)
            spawn_premium(
                sub,N["Omnis_Bush"],
                (x+rng.uniform(-220,220),y+rng.uniform(-220,220),25),
                (b,b,b),"Premium_Bush_%03d"%i,
                mat["Nature"],rng.uniform(0,360))

        if i%4==0:
            r=rng.uniform(0.7,1.8)
            spawn_premium(
                sub,N["Omnis_Rock"],(x+180,y-160,25),
                (r,r,r*0.8),"Premium_Rock_%03d"%i,
                mat["Nature"],rng.uniform(0,360))

    for i in range(18):
        x=-9000+i*900
        spawn_premium(
            sub,I["Omnis_Street_Lamp"],(x,6800,45),(1,1,1),
            "Premium_Lamp_%02d"%i,mat["Infrastructure"])
        spawn_premium(
            sub,I["Omnis_Fence"],(x,6600,30),(2,1,1),
            "Premium_Fence_%02d"%i,mat["Infrastructure"])

    O=base.require_mesh("Obelisk","Omnis_Obelisk")
    for i,(x,y,s) in enumerate([
        (-15000,-10500,3),(15000,10800,3.2),
        (15000,-8500,2.6),(-15000,9500,2.8)]):
        spawn_premium(
            sub,O,(x,y,70),(s,s,s),
            "Premium_Cycle_Landmark_%02d"%i,mat["Obelisk"])

def main():
    base.ensure_dirs()
    base.import_all()
    import_obelisk()
    mats=materials()
    meshes={
        f:{n:base.require_mesh(f,n) for n in ns}
        for f,ns in base.FAMILIES.items()
    }

    levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    sub=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    if not unreal.EditorAssetLibrary.does_asset_exist(TEMPLATE):
        raise RuntimeError("UE5.8 Open World template missing: " + TEMPLATE)

    if not levels.new_level_from_template(MAP_PATH,TEMPLATE):
        raise RuntimeError("Failed to materialize Open World template: " + MAP_PATH)

    world=unreal.EditorLevelLibrary.get_editor_world()
    if not world or not world.get_world_partition():
        raise RuntimeError(
            "World Partition was not materialized by the Open World template")

    base.populate(sub)
    extra_dressing(sub,meshes,mats)
    base.add_lighting(sub)

    if not levels.save_current_level():
        raise RuntimeError("Failed to save " + MAP_PATH)

    actors=unreal.EditorLevelLibrary.get_all_level_actors()
    count=sum(
        1 for actor in actors
        if actor and isinstance(actor,unreal.StaticMeshActor))
    expected=sum(len(v) for v in base.FAMILIES.values())
    found=sum(
        1 for f,ns in base.FAMILIES.items()
        for n in ns
        if unreal.EditorAssetLibrary.load_asset(base.asset_path(f,n)))
    mat_count=sum(
        1 for f in COLORS
        if unreal.EditorAssetLibrary.load_asset(
            MAT_ROOT+"/M_"+f+"_Premium"))

    if not unreal.Paths.file_exists(
        ROOT+"Content/Omnis/Maps/Omnis_OpenWorld.umap"):
        raise RuntimeError("Native .umap missing")
    if found<expected:
        raise RuntimeError("Native mesh library incomplete")
    if mat_count<9:
        raise RuntimeError("Premium materials incomplete")
    if count<250:
        raise RuntimeError("World dressing incomplete: %d actors"%count)

    log(
        "VALIDATION OK: World Partition + meshes=%d/%d + materials=%d + static_actors=%d"
        %(found,expected,mat_count,count))
    log("REAL PREMIUM WORLD MATERIALIZED")

if __name__=="__main__":
    main()
