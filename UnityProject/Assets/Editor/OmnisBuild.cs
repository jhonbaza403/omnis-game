using System;
using System.IO;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEngine;

namespace Omnis.Editor
{
    /// <summary>
    /// Explicit build entry point for CI. It refuses to produce a build until
    /// a real Omnis main scene has been migrated into the Unity project.
    /// </summary>
    public static class OmnisBuild
    {
        public static void BuildLinux()
        {
            const string mainScene = "Assets/Scenes/OmnisMain.unity";
            if (!File.Exists(mainScene))
            {
                throw new InvalidOperationException(
                    "Omnis migration incomplete: Assets/Scenes/OmnisMain.unity is missing. " +
                    "Do not generate a placeholder scene to make CI appear successful.");
            }

            EditorBuildSettings.scenes = new[]
            {
                new EditorBuildSettingsScene(mainScene, true)
            };

            Directory.CreateDirectory("Builds/Linux");
            var options = new BuildPlayerOptions
            {
                scenes = new[] { mainScene },
                locationPathName = "Builds/Linux/Omnis.x86_64",
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
