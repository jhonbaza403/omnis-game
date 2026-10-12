using System;
using UnityEditor;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;

namespace Omnis.Editor
{
    /// <summary>Idempotently creates and applies Omnis' high-quality URP baseline.</summary>
    [InitializeOnLoad]
    public static class OmnisGraphicsSetup
    {
        public const string PipelinePath = "Assets/Omnis/Settings/Omnis_URP_4K.asset";
        public const string RendererPath = "Assets/Omnis/Settings/Omnis_ForwardPlus_Renderer.asset";

        static OmnisGraphicsSetup()
        {
            EditorApplication.delayCall += () =>
            {
                try
                {
                    EnsureConfigured();
                    if (!System.IO.File.Exists(System.IO.Path.Combine(Application.dataPath, "Scenes", "OmnisMain.unity")))
                        OmnisSceneBootstrap.CreateMainScene();
                }
                catch (Exception exception)
                {
                    Debug.LogWarning("[Omnis] Automatic graphics setup is deferred until Unity packages finish resolving: " + exception.Message);
                }
            };
        }

        [MenuItem("Omnis/Graphics/Configure 4K URP Quality")]
        public static void EnsureConfigured()
        {
            EnsureFolder("Assets/Omnis/Settings");

            UniversalRendererData rendererData = AssetDatabase.LoadAssetAtPath<UniversalRendererData>(RendererPath);
            if (rendererData == null)
            {
                rendererData = ScriptableObject.CreateInstance<UniversalRendererData>();
                rendererData.name = "Omnis Forward Plus Renderer";
                rendererData.renderingMode = RenderingMode.ForwardPlus;
                AssetDatabase.CreateAsset(rendererData, RendererPath);
            }

            UniversalRenderPipelineAsset pipeline = AssetDatabase.LoadAssetAtPath<UniversalRenderPipelineAsset>(PipelinePath);
            if (pipeline == null)
            {
                pipeline = UniversalRenderPipelineAsset.Create(rendererData);
                pipeline.name = "Omnis URP 4K";
                AssetDatabase.CreateAsset(pipeline, PipelinePath);
            }

            // Use public pipeline settings; lower graphics tiers can be added after performance profiling.
            pipeline.supportsHDR = true;
            pipeline.renderScale = 1f;
            pipeline.msaaSampleCount = 4;
            pipeline.shadowDistance = 155f;
            ConfigureSerializedPipelineSettings(pipeline);
            pipeline.maxAdditionalLightsCount = 6;
            pipeline.supportsCameraDepthTexture = true;
            pipeline.supportsCameraOpaqueTexture = true;

            rendererData.renderingMode = RenderingMode.ForwardPlus;
            EditorUtility.SetDirty(rendererData);
            EditorUtility.SetDirty(pipeline);

            GraphicsSettings.defaultRenderPipeline = pipeline;
            QualitySettings.renderPipeline = pipeline;
            QualitySettings.shadows = ShadowQuality.All;
            QualitySettings.shadowResolution = ShadowResolution.VeryHigh;
            QualitySettings.shadowDistance = 155f;
            QualitySettings.antiAliasing = 4;
            QualitySettings.anisotropicFiltering = AnisotropicFiltering.ForceEnable;
            QualitySettings.lodBias = 2f;
            QualitySettings.maximumLODLevel = 0;
            QualitySettings.pixelLightCount = 8;
            QualitySettings.realtimeReflectionProbes = true;
            QualitySettings.streamingMipmapsActive = true;
            QualitySettings.vSyncCount = 1;

            PlayerSettings.companyName = "Omnis Game Studio";
            PlayerSettings.productName = "Omnis: El Último Ciclo";
#pragma warning disable CS0618
            // Value 2 keeps both legacy and Input System backends enabled; the game uses Input System.
            PlayerSettings.SetPropertyInt("activeInputHandler", 2, BuildTargetGroup.Unknown);
#pragma warning restore CS0618
            PlayerSettings.colorSpace = ColorSpace.Linear;
            PlayerSettings.defaultScreenWidth = 3840;
            PlayerSettings.defaultScreenHeight = 2160;
            PlayerSettings.defaultIsNativeResolution = false;
            PlayerSettings.fullScreenMode = FullScreenMode.FullScreenWindow;
            PlayerSettings.runInBackground = true;

            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log($"[Omnis] URP Forward+ configured in Unity {Application.unityVersion}. Standalone default: 3840x2160; runtime performance must still be measured on target hardware.");
        }

        private static void ConfigureSerializedPipelineSettings(UniversalRenderPipelineAsset pipeline)
        {
            // URP keeps some shadow flags read-only because they affect shader stripping.
            // Set persistent Editor defaults through SerializedObject instead.
            SerializedObject serialized = new SerializedObject(pipeline);
            SetBoolean(serialized, "m_MainLightShadowsSupported", true);
            SetBoolean(serialized, "m_AdditionalLightShadowsSupported", true);
            SetBoolean(serialized, "m_SoftShadowsSupported", true);

            SerializedProperty resolution = serialized.FindProperty("m_MainLightShadowmapResolution");
            if (resolution != null) resolution.intValue = 4096;
            SerializedProperty additionalResolution = serialized.FindProperty("m_AdditionalLightsShadowmapResolution");
            if (additionalResolution != null) additionalResolution.intValue = 1024;
            serialized.ApplyModifiedPropertiesWithoutUndo();
        }

        private static void SetBoolean(SerializedObject serialized, string propertyName, bool value)
        {
            SerializedProperty property = serialized.FindProperty(propertyName);
            if (property != null) property.boolValue = value;
        }

        private static void EnsureFolder(string folderPath)
        {
            if (AssetDatabase.IsValidFolder(folderPath)) return;
            string parent = System.IO.Path.GetDirectoryName(folderPath)?.Replace('\\', '/');
            string name = System.IO.Path.GetFileName(folderPath);
            if (string.IsNullOrEmpty(parent)) throw new InvalidOperationException("Invalid Unity asset folder: " + folderPath);
            EnsureFolder(parent);
            if (!AssetDatabase.IsValidFolder(folderPath)) AssetDatabase.CreateFolder(parent, name);
        }
    }
}
