using System;
using System.IO;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;

public static class OmnisMaterialization
{
    const string SourceRoot = "Content/Omnis/SourceAssets";
    const string UnityRoot = "Assets/Omnis/SourceAssets";
    const string ScenePath = "Assets/Omnis/Scenes/Omnis_Nexus_Prototype.unity";
    [MenuItem("Omnis/Materialize/Import Verified Source Assets")]
    public static void ImportVerifiedSourceAssets()
    {
        if (!Directory.Exists(SourceRoot)) throw new DirectoryNotFoundException(SourceRoot);
        Directory.CreateDirectory(UnityRoot);
        int copied = 0;
        foreach (var source in Directory.GetFiles(SourceRoot, "*", SearchOption.AllDirectories))
        {
            if (source.EndsWith(".meta", StringComparison.OrdinalIgnoreCase)) continue;
            var relative = source.Substring(SourceRoot.Length).TrimStart(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
            var destination = Path.Combine(UnityRoot, relative);
            Directory.CreateDirectory(Path.GetDirectoryName(destination)!);
            if (!File.Exists(destination) || !FilesEqual(source, destination)) { File.Copy(source, destination, true); copied++; }
        }
        AssetDatabase.Refresh(ImportAssetOptions.ForceSynchronousImport);
        Debug.Log("Omnis: imported " + copied + " verified source files.");
    }

    [MenuItem("Omnis/Materialize/Create First Playable Scene")]
    public static void CreateFirstPlayableScene()
    {
        ImportVerifiedSourceAssets();
        var scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
        CreateLight();
        var player = AddImportedModel("Omnis_Civilian", new Vector3(0, 0, -8));
        if (player != null) SetupPlayer(player);
        AddImportedModel("Omnis_Obelisk", new Vector3(0, 0, 8));
        AddImportedModel("Omnis_Road_Segment", new Vector3(0, 0, 0));
        AddImportedModel("Omnis_Tree", new Vector3(7, 0, 8));
        AddImportedModel("Omnis_House", new Vector3(-7, 0, 8));
        EditorSceneManager.SaveScene(scene, ScenePath);
        Debug.Log("Omnis: first playable source-asset scene saved to " + ScenePath);
    }

    static void SetupPlayer(GameObject player)
    {
        player.name = "OmnisPlayer";
        var controller = player.GetComponent<CharacterController>();
        if (controller == null) controller = player.AddComponent<CharacterController>();
        controller.height = 1.8f; controller.radius = 0.35f; controller.center = new Vector3(0, 0.9f, 0);
        var movement = player.GetComponent<OmnisPlayerController>();
        if (movement == null) movement = player.AddComponent<OmnisPlayerController>();
        var cameraObject = new GameObject("Player Camera");
        var camera = cameraObject.AddComponent<Camera>();
        camera.tag = "MainCamera";
        cameraObject.transform.SetParent(player.transform);
        cameraObject.transform.localPosition = new Vector3(0, 2.2f, -5.5f);
        cameraObject.transform.localRotation = Quaternion.Euler(12, 0, 0);
        movement.cameraPivot = cameraObject.transform;
    }

    static void CreateLight()
    {
        var go = new GameObject("Key Light");
        var light = go.AddComponent<Light>();
        light.type = LightType.Directional; light.intensity = 1.2f;
        go.transform.rotation = Quaternion.Euler(45, -30, 0);
    }

    static GameObject AddImportedModel(string modelName, Vector3 position)
    {
        string[] guids = AssetDatabase.FindAssets(modelName + " t:Model", new[] { UnityRoot });
        if (guids.Length == 0) { Debug.LogWarning("Omnis: model not found: " + modelName); return null; }
        string path = AssetDatabase.GUIDToAssetPath(guids[0]);
        var prefab = AssetDatabase.LoadAssetAtPath<GameObject>(path);
        if (prefab == null) { Debug.LogWarning("Omnis: unable to load: " + path); return null; }
        var instance = PrefabUtility.InstantiatePrefab(prefab) as GameObject;
        if (instance == null) return null;
        instance.name = modelName; instance.transform.position = position;
        return instance;
    }

    static bool FilesEqual(string a, string b)
    {
        var ai = new FileInfo(a); var bi = new FileInfo(b);
        if (ai.Length != bi.Length) return false;
        using var af = File.OpenRead(a); using var bf = File.OpenRead(b);
        int x, y; do { x = af.ReadByte(); y = bf.ReadByte(); if (x != y) return false; } while (x != -1);
        return true;
    }
}