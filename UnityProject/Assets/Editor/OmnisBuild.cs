using System;
using System.IO;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEngine;

namespace Omnis.Editor
{
    /// <summary>
    /// CI build entry point. Refuses to produce a build until the real Omnis
    /// main scene has been migrated into this Unity project.
    /// </summary>
    public static class OmnisBuild
    {
        public static void BuildLinux()
        {
            const string mainScene = "Assets/Scenes/OmnisMain.unity";
            if (!File.Exists(Path.Combine(Application.dataPath, "Scenes", "OmnisMain.unity")))
            {
                throw new InvalidOperationException(
                    "Omnis migration incomplete: Assets/Scenes/OmnisMain.unity is missing. " +
                    "Do not generate a placeholder scene to make CI appear successful.");
            }

            var projectRoot = Directory.GetParent(Application.dataPath)?.FullName
                ?? throw new InvalidOperationException("Unable to resolve Unity project root.");
            var outputDirectory = Path.Combine(projectRoot, "Builds", "Linux");
            Directory.CreateDirectory(outputDirectory);

            EditorBuildSettings.scenes = new[]
            {
                new EditorBuildSettingsScene(mainScene, true)
            };

            var options = new BuildPlayerOptions
            {
                scenes = new[] { mainScene },
                locationPathName = Path.Combine(outputDirectory, "Omnis.x86_64"),
                target = BuildTarget.StandaloneLinux64,
                options = BuildOptions.StrictMode
            };

            BuildReport report = BuildPipeline.BuildPlayer(options);
            if (report.summary.result != BuildResult.Succeeded)
            {
                throw new InvalidOperationException(
                    $"Unity build failed: {report.summary.result}; " +
                    $"errors={report.summary.totalErrors}, warnings={report.summary.totalWarnings}");
            }

            Debug.Log($"OMNIS UNITY BUILD SUCCEEDED: {report.summary.totalSize} bytes");
        }
    }
}
