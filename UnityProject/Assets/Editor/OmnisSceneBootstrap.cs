using System.IO;
using Omnis;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace Omnis.Editor
{
    /// <summary>Creates and saves a genuine Unity scene with a small playable primitive-based slice.</summary>
    public static class OmnisSceneBootstrap
    {
        private const string ScenePath = "Assets/Scenes/OmnisMain.unity";
        private const string MaterialsPath = "Assets/Omnis/Materials";

        public static void CreateMainScene()
        {
            string sceneAbsolutePath = Path.Combine(Application.dataPath, "Scenes", "OmnisMain.unity");
            if (File.Exists(sceneAbsolutePath))
            {
                Scene existing = EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);
                EditorBuildSettings.scenes = new[] { new EditorBuildSettingsScene(ScenePath, true) };
                Debug.Log("Omnis scene opened: " + existing.path);
                return;
            }

            Directory.CreateDirectory(Path.Combine(Application.dataPath, "Scenes"));
            Directory.CreateDirectory(Path.Combine(Application.dataPath, "Omnis", "Materials"));
            AssetDatabase.Refresh();
            Scene scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
            scene.name = "OmnisMain";

            Material groundMat = CreateMaterial("OmnisGround.mat", new Color(0.055f, 0.075f, 0.095f));
            Material obeliskMat = CreateMaterial("OmnisNexus.mat", new Color(0.12f, 0.32f, 0.38f));
            Material coreMat = CreateMaterial("OmnisNexusCore.mat", new Color(0.25f, 0.95f, 0.85f));
            Material playerMat = CreateMaterial("OmnisWandererPrototype.mat", new Color(0.65f, 0.72f, 0.82f));

            new GameObject("Omnis Game State").AddComponent<OmnisGameState>();

            GameObject ground = GameObject.CreatePrimitive(PrimitiveType.Cube);
            ground.name = "Nexus Ground";
            ground.transform.position = new Vector3(0f, -0.5f, 5f);
            ground.transform.localScale = new Vector3(60f, 1f, 60f);
            ground.GetComponent<Renderer>().sharedMaterial = groundMat;

            GameObject player = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            player.name = "Omnis Wanderer";
            player.tag = "Player";
            player.transform.position = new Vector3(0f, 1.05f, -4f);
            Object.DestroyImmediate(player.GetComponent<CapsuleCollider>());
            CharacterController cc = player.AddComponent<CharacterController>();
            cc.height = 1.8f;
            cc.radius = 0.38f;
            cc.center = Vector3.zero;
            player.GetComponent<Renderer>().sharedMaterial = playerMat;

            GameObject pivot = new GameObject("Camera Pivot");
            pivot.transform.SetParent(player.transform, false);
            pivot.transform.localPosition = new Vector3(0f, 0.65f, 0f);

            GameObject cameraObject = new GameObject("Main Camera");
            cameraObject.tag = "MainCamera";
            cameraObject.transform.SetParent(pivot.transform, false);
            cameraObject.transform.localPosition = new Vector3(0f, 0.9f, -5.5f);
            cameraObject.transform.localRotation = Quaternion.Euler(8f, 0f, 0f);
            Camera camera = cameraObject.AddComponent<Camera>();
            camera.fieldOfView = 68f;
            camera.nearClipPlane = 0.1f;
            camera.farClipPlane = 1200f;
            cameraObject.AddComponent<AudioListener>();

            OmnisPlayerController pc = player.AddComponent<OmnisPlayerController>();
            SetPrivateField(pc, "cameraPivot", pivot.transform);
            SetPrivateField(pc, "playerCamera", camera);

            GameObject obelisk = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            obelisk.name = "Nexus Obelisk - Interactable";
            obelisk.transform.position = new Vector3(0f, 1.6f, 7f);
            obelisk.transform.localScale = new Vector3(1.25f, 1.6f, 1.25f);
            obelisk.GetComponent<Renderer>().sharedMaterial = obeliskMat;
            OmnisInteractable interactable = obelisk.AddComponent<OmnisInteractable>();
            SetPrivateField(interactable, "interactionId", "Omnis.Nexus.Obelisk");
            SetPrivateField(interactable, "displayName", "Obelisco del Nexo");
            SetPrivateField(interactable, "experienceReward", 25);
            SetPrivateField(interactable, "entersNexus", true);

            GameObject core = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            core.name = "Nexus Core";
            core.transform.position = new Vector3(0f, 4.2f, 7f);
            core.transform.localScale = Vector3.one * 0.7f;
            core.GetComponent<Renderer>().sharedMaterial = coreMat;
            Object.DestroyImmediate(core.GetComponent<Collider>());

            GameObject lightObject = new GameObject("Nexus Key Light");
            Light light = lightObject.AddComponent<Light>();
            light.type = LightType.Directional;
            light.intensity = 1.25f;
            lightObject.transform.rotation = Quaternion.Euler(42f, -28f, 0f);

            OmnisHud hud = new GameObject("Omnis HUD").AddComponent<OmnisHud>();
            SetPrivateField(hud, "player", pc);

            EditorSceneManager.MarkSceneDirty(scene);
            if (!EditorSceneManager.SaveScene(scene, ScenePath))
                throw new System.InvalidOperationException("Unity failed to save the Omnis main scene.");
            EditorBuildSettings.scenes = new[] { new EditorBuildSettingsScene(ScenePath, true) };
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log("OMNIS MAIN SCENE MATERIALIZED by Unity Editor: " + ScenePath);
        }

        private static Material CreateMaterial(string fileName, Color color)
        {
            string path = MaterialsPath + "/" + fileName;
            Material existing = AssetDatabase.LoadAssetAtPath<Material>(path);
            if (existing != null) { existing.color = color; EditorUtility.SetDirty(existing); return existing; }
            Shader shader = Shader.Find("Universal Render Pipeline/Lit");
            if (shader == null) shader = Shader.Find("Standard");
            if (shader == null) throw new System.InvalidOperationException("No supported Unity shader was found.");
            Material material = new Material(shader) { color = color };
            AssetDatabase.CreateAsset(material, path);
            return material;
        }

        private static void SetPrivateField<T>(object target, string fieldName, T value)
        {
            var field = target.GetType().GetField(fieldName,
                System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.NonPublic);
            if (field == null) throw new System.MissingFieldException(target.GetType().FullName, fieldName);
            field.SetValue(target, value);
        }
    }
}
