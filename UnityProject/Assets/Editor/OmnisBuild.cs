using System;
using System.IO;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEngine;

namespace Omnis.Editor
{
    public static class OmnisBuild
    {
        public static void BuildLinux()
        {
            OmnisSceneBootstrap.CreateMainScene();
            const string mainScene = "Assets/Scenes/OmnisMain.unity";
            if (!File.Exists(Path.Combine(Application.dataPath, "Scenes", "OmnisMain.unity")))
                throw new InvalidOperationException("Unity Editor did not materialize the Omnis main scene.");

            string projectRoot = Directory.GetParent(Application.dataPath)?.FullName
                ?? throw new InvalidOperationException("Unable to resolve Unity project root.");
            string outputDirectory = Path.Combine(projectRoot, "Builds", "Linux");
            Directory.CreateDirectory(outputDirectory);

            EditorBuildSettings.scenes = new[] { new EditorBuildSettingsScene(mainScene, true) };
            BuildPlayerOptions options = new BuildPlayerOptions
            {
                scenes = new[] { mainScene },
                locationPathName = Path.Combine(outputDirectory, "Omnis.x86_64"),
                target = BuildTarget.StandaloneLinux64,
                options = BuildOptions.StrictMode
            };
            BuildReport report = BuildPipeline.BuildPlayer(options);
            if (report.summary.result != BuildResult.Succeeded)
                throw new InvalidOperationException($"Unity build failed: {report.summary.result}; errors={report.summary.totalErrors}, warnings={report.summary.totalWarnings}");
            Debug.Log($"OMNIS UNITY BUILD SUCCEEDED: {report.summary.totalSize} bytes");
        }
    }
}
