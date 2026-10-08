#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace Omnis.Editor
{
    public static class OmnisPremiumAssetComposer
    {
        const string Root = "OMNIS_REAL_ASSETS";
        const string House = "Assets/Art/ImportedFromUnreal/Architecture/Omnis_House.obj";
        const string Civilian = "Assets/Art/ImportedFromUnreal/Characters/Omnis_Civilian.obj";
        const string Tree = "Assets/Art/ImportedFromUnreal/Nature/Omnis_Tree.obj";
        const string NexusTexture = "Assets/Art/ImportedFromUnreal/Textures/Omnis_Nexus_Albedo.png";

        [MenuItem("Omnis/Build Premium Real Asset Composition")]
        public static void Build()
        {
            EnsureFolder("Assets/Prefabs/ImportedFromUnreal");

            var old = GameObject.Find(Root);
            if (old != null) Object.DestroyImmediate(old);

            var root = new GameObject(Root);

            PlaceModel(House, root.transform, new Vector3(24f, 0f, 24f), Quaternion.identity, "REAL_House");
            PlaceModel(Civilian, root.transform, new Vector3(4f, 0f, 10f), Quaternion.Euler(0f, 180f, 0f), "REAL_Civilian");
            PlaceModel(Tree, root.transform, new Vector3(-18f, 0f, 16f), Quaternion.identity, "REAL_Tree_A");
            PlaceModel(Tree, root.transform, new Vector3(-26f, 0f, 10f), Quaternion.Euler(0f, 42f, 0f), "REAL_Tree_B");

            var texture = AssetDatabase.LoadAssetAtPath<Texture2D>(NexusTexture);
            if (texture != null)
                BuildNexusTextureMaterial(texture);

            EditorSceneManager.MarkSceneDirty(SceneManager.GetActiveScene());
            EditorSceneManager.SaveScene(SceneManager.GetActiveScene());
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();

            Debug.Log("[Omnis] Real Unreal-exported assets composed into the active Unity scene.");
        }

        static void PlaceModel(string path, Transform parent, Vector3 position, Quaternion rotation, string name)
        {
            var source = AssetDatabase.LoadAssetAtPath<GameObject>(path);
            if (source == null)
            {
                Debug.LogWarning("[Omnis] Asset not imported yet: " + path);
                return;
            }

            var instance = (GameObject)PrefabUtility.InstantiatePrefab(source);
            instance.name = name;
            instance.transform.SetParent(parent, false);
            instance.transform.localPosition = position;
            instance.transform.localRotation = rotation;

            var prefabPath = "Assets/Prefabs/ImportedFromUnreal/" + name + ".prefab";
            PrefabUtility.SaveAsPrefabAssetAndConnect(instance, prefabPath, InteractionMode.AutomatedAction);
        }

        static void BuildNexusTextureMaterial(Texture2D texture)
        {
            var shader = Shader.Find("Universal Render Pipeline/Lit") ?? Shader.Find("Standard");
            if (shader == null) return;

            var material = new Material(shader) { name = "MAT_Nexus_Albedo_Real" };
            if (material.HasProperty("_BaseMap")) material.SetTexture("_BaseMap", texture);
            if (material.HasProperty("_MainTex")) material.SetTexture("_MainTex", texture);
            AssetDatabase.CreateAsset(material, "Assets/Materials/MAT_Nexus_Albedo_Real.mat");
        }

        static void EnsureFolder(string path)
        {
            var parts = path.Split('/');
            var current = parts[0];
            for (int i = 1; i < parts.Length; i++)
            {
                var next = current + "/" + parts[i];
                if (!AssetDatabase.IsValidFolder(next))
                    AssetDatabase.CreateFolder(current, parts[i]);
                current = next;
            }
        }
    }
}
#endif
