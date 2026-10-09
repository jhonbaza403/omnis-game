using System.IO;
using Omnis;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace Omnis.Editor
{
    /// <summary>Guarantees that the committed main scene exists and contains the runtime world bootstrap.</summary>
    public static class OmnisSceneBootstrap
    {
        public const string ScenePath = "Assets/Scenes/OmnisMain.unity";

        [MenuItem("Omnis/Scene/Ensure Main Playable Scene")]
        public static void CreateMainScene()
        {
            string sceneAbsolutePath = Path.Combine(Application.dataPath, "Scenes", "OmnisMain.unity");
            Directory.CreateDirectory(Path.GetDirectoryName(sceneAbsolutePath) ?? Application.dataPath + "/Scenes");

            Scene scene;
            if (File.Exists(sceneAbsolutePath))
            {
                scene = EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);
            }
            else
            {
                scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
                scene.name = "OmnisMain";
            }

            bool hasBootstrap = false;
            foreach (GameObject root in scene.GetRootGameObjects())
            {
                if (root.GetComponent<OmnisWorldBootstrap>() != null)
                {
                    hasBootstrap = true;
                    break;
                }
            }

            if (!hasBootstrap)
            {
                GameObject bootstrap = new GameObject("OMNIS Runtime World");
                bootstrap.AddComponent<OmnisWorldBootstrap>();
                EditorSceneManager.MarkSceneDirty(scene);
            }

            if (!EditorSceneManager.SaveScene(scene, ScenePath))
                throw new System.InvalidOperationException("Unity could not save " + ScenePath);

            EditorBuildSettings.scenes = new[] { new EditorBuildSettingsScene(ScenePath, true) };
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log("[Omnis] Main scene is saved and enabled for builds: " + ScenePath);
        }
    }
}
