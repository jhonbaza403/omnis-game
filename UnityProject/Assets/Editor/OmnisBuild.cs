using System;
using System.IO;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;
using UnityEngine.SceneManagement;

namespace Omnis.Editor
{
    public static class OmnisBuild
    {
        private const string MainScene = OmnisSceneBootstrap.ScenePath;

        [Serializable]
        private sealed class ReleaseManifest
        {
            public string game;
            public string unityEditorVersion;
            public string target;
            public string createdUtc;
            public string resolution;
            public string renderPipeline;
            public string buildResult;
            public long buildSizeBytes;
            public int errors;
            public int warnings;
        }

        [MenuItem("Omnis/Build/Build Linux 64-bit")]
        public static void BuildLinux() => BuildPlayer(BuildTarget.StandaloneLinux64, "Linux", "Omnis.x86_64");

        [MenuItem("Omnis/Build/Build Windows 64-bit")]
        public static void BuildWindows() => BuildPlayer(BuildTarget.StandaloneWindows64, "Windows", "Omnis.exe");

        [MenuItem("Omnis/Build/Validate Release Configuration")]
        public static void ValidateProject()
        {
            OmnisGraphicsSetup.EnsureConfigured();
            OmnisSceneBootstrap.CreateMainScene();
            AssetDatabase.Refresh();

            if (!File.Exists(Path.Combine(Application.dataPath, "Scenes", "OmnisMain.unity")))
                throw new InvalidOperationException("Main scene file is missing.");

            SceneAsset sceneAsset = AssetDatabase.LoadAssetAtPath<SceneAsset>(MainScene);
            if (sceneAsset == null)
                throw new InvalidOperationException("Unity cannot import the main scene asset: " + MainScene);

            if (!(GraphicsSettings.defaultRenderPipeline is UniversalRenderPipelineAsset))
                throw new InvalidOperationException("Universal Render Pipeline is not the active graphics pipeline.");

            Scene scene = SceneManager.GetActiveScene();
            bool hasWorldBootstrap = false;
            foreach (GameObject root in scene.GetRootGameObjects())
            {
                if (root.GetComponent<OmnisWorldBootstrap>() != null)
                {
                    hasWorldBootstrap = true;
                    break;
                }
            }
            if (!hasWorldBootstrap)
                throw new InvalidOperationException("Main scene does not contain OmnisWorldBootstrap.");

            RequireAsset("Assets/Omnis/Scripts/OmnisPlayerController.cs");
            RequireAsset("Assets/Omnis/Scripts/OmnisGameState.cs");
            RequireAsset("Assets/Omnis/Scripts/OmnisHud.cs");
            RequireAsset("Assets/Omnis/Scripts/OmnisInteractable.cs");
            RequireAsset("Assets/Omnis/Scripts/OmnisSentinel.cs");
            RequireAsset("Assets/Omnis/Scripts/OmnisWorldBootstrap.cs");

            Debug.Log("OMNIS UNITY RELEASE VALIDATION PASSED: scene imports, runtime bootstrap is present, and URP is active.");
        }

        private static void RequireAsset(string path)
        {
            if (AssetDatabase.LoadMainAssetAtPath(path) == null)
                throw new InvalidOperationException("Required Unity source asset is missing: " + path);
        }

        private static void BuildPlayer(BuildTarget target, string targetDirectoryName, string executableName)
        {
            OmnisGraphicsSetup.EnsureConfigured();
            OmnisSceneBootstrap.CreateMainScene();
            AssetDatabase.Refresh();

            string projectRoot = Directory.GetParent(Application.dataPath)?.FullName
                ?? throw new InvalidOperationException("Unable to resolve Unity project root.");
            string outputDirectory = Path.Combine(projectRoot, "Builds", targetDirectoryName);
            Directory.CreateDirectory(outputDirectory);

            EditorBuildSettings.scenes = new[] { new EditorBuildSettingsScene(MainScene, true) };
            BuildPlayerOptions options = new BuildPlayerOptions
            {
                scenes = new[] { MainScene },
                locationPathName = Path.Combine(outputDirectory, executableName),
                target = target,
                options = BuildOptions.StrictMode
            };

            BuildReport report = BuildPipeline.BuildPlayer(options);
            BuildSummary summary = report.summary;
            if (summary.result != BuildResult.Succeeded)
            {
                throw new InvalidOperationException(
                    $"Unity build failed for {target}: {summary.result}; errors={summary.totalErrors}, warnings={summary.totalWarnings}");
            }

            ReleaseManifest manifest = new ReleaseManifest
            {
                game = "Omnis: El Último Ciclo",
                unityEditorVersion = Application.unityVersion,
                target = target.ToString(),
                createdUtc = DateTime.UtcNow.ToString("O"),
                resolution = "3840x2160",
                renderPipeline = "URP 17 / Forward+",
                buildResult = summary.result.ToString(),
                buildSizeBytes = summary.totalSize,
                errors = summary.totalErrors,
                warnings = summary.totalWarnings
            };
            File.WriteAllText(Path.Combine(outputDirectory, "omnis-release-manifest.json"),
                JsonUtility.ToJson(manifest, true) + Environment.NewLine);

            Debug.Log($"OMNIS BUILD SUCCEEDED: target={target}; bytes={summary.totalSize}; errors={summary.totalErrors}; warnings={summary.totalWarnings}");
            Debug.Log("Release output: " + outputDirectory);
        }
    }
}
