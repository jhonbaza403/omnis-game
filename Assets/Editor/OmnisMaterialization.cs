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
        CreateCamera();
        AddImportedModel("Omnis_Obelisk", new Vector3(0, 0, 8));
        AddImportedModel("Omnis_Road_Segment", new Vector3(0, 0, 0));
        AddImportedModel("Omnis_Tree", new Vector3(7, 0, 8));
        AddImportedModel("Omnis_House", new Vector3(-7, 0, 8));
        EditorSceneManager.SaveScene(scene, ScenePath);
        Debug.Log("Omnis: first real source-asset scene saved to " + ScenePath);
    }

    static void CreateCamera()
    {
        var go = new GameObject("Main Camera");
        go.tag = "MainCamera";
        go.AddComponent<Camera>();
        go.transform.position = new Vector3(0, 4, -12);
        go.transform.rotation = Quaternion.Euler(15, 0, 0);
    }

    static void CreateLight()
    {
        var go = new GameObject("Key Light");
        var light = go.AddComponent<Light>();
        light.type = LightType.Directional;
        light.intensity = 1.2f;
        go.transform.rotation = Quaternion.Euler(45, -30, 0);
    }

    static void AddImportedModel(string modelName, Vector3 position)
    {
        string[] guids = AssetDatabase.FindAssets(modelName + " t:Model", new[] { UnityRoot });
        if (guids.Length == 0) { Debug.LogWarning("Omnis: model not found: " + modelName); return; }
        string path = AssetDatabase.GUIDToAssetPath(guids[0]);
        var prefab = AssetDatabase.LoadAssetAtPath<GameObject>(path);
        if (prefab == null) { Debug.LogWarning("Omnis: unable to load: " + path); return; }
        var instance = PrefabUtility.InstantiatePrefab(prefab) as GameObject;
        if (instance == null) return;
        instance.name = modelName;
        instance.transform.position = position;
    }

    static bool FilesEqual(string a, string b)
    {
        var ai = new FileInfo(a); var bi = new FileInfo(b);
        if (ai.Length != bi.Length) return false;
        using var af = File.OpenRead(a); using var bf = File.OpenRead(b);
        int x, y;
        do { x = af.ReadByte(); y = bf.ReadByte(); if (x != y) return false; } while (x != -1);
        return true;
    }
}