using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;

namespace Omnis
{
    /// <summary>
    /// Materializes a stylized, self-contained playable world at runtime.
    /// It uses deterministic meshes/primitives rather than fake references to unavailable Git LFS objects.
    /// </summary>
    public sealed class OmnisWorldBootstrap : MonoBehaviour
    {
        private readonly Dictionary<string, Material> materials = new Dictionary<string, Material>();
        private readonly List<UnityEngine.Object> generatedRuntimeAssets = new List<UnityEngine.Object>();
        private Transform worldRoot;
        private System.Random random;

        private void Awake()
        {
            if (GameObject.Find("OMNIS_RUNTIME_WORLD") != null) return;

            random = new System.Random(20261008);
            Application.targetFrameRate = 60;
            QualitySettings.vSyncCount = 1;
            QualitySettings.antiAliasing = 4;
            QualitySettings.anisotropicFiltering = AnisotropicFiltering.Enable;
            BuildWorld();
        }

        private void BuildWorld()
        {
            worldRoot = new GameObject("OMNIS_RUNTIME_WORLD").transform;
            var stateObject = new GameObject("Omnis Game State");
            stateObject.transform.SetParent(worldRoot, false);
            stateObject.AddComponent<OmnisGameState>();

            CreateMaterials();
            ConfigureAtmosphere();
            GameObject simulationObject = new GameObject("Omnis World Simulation");
            simulationObject.transform.SetParent(worldRoot, false);
            simulationObject.AddComponent<OmnisWorldSimulation>();
            GameObject audioObject = new GameObject("Omnis Audio Director");
            audioObject.transform.SetParent(worldRoot, false);
            audioObject.AddComponent<OmnisAudioDirector>();
            BuildGroundAndPath();
            BuildNexus();
            BuildRuins();
            BuildNature();
            BuildOpenWorldExpansion();
            BuildRelicsAndSentinels();
            BuildPlayer();
            BuildAmbientParticles();
        }

        private void CreateMaterials()
        {
            materials["ground"] = MakeMaterial("Nexus Basalt", new Color(0.075f, 0.105f, 0.15f), 0.12f, 0.28f);
            materials["stone"] = MakeMaterial("Ancient Stone", new Color(0.16f, 0.21f, 0.29f), 0.22f, 0.44f);
            materials["stoneLight"] = MakeMaterial("Silver Slate", new Color(0.31f, 0.37f, 0.45f), 0.18f, 0.52f);
            materials["gold"] = MakeMaterial("Nexus Gold", new Color(0.76f, 0.43f, 0.12f), 0.62f, 0.7f);
            materials["cyan"] = MakeMaterial("Aether Cyan", new Color(0.12f, 0.8f, 0.88f), 0.22f, 0.62f, new Color(0.06f, 0.82f, 1f) * 2.4f);
            materials["violet"] = MakeMaterial("Rift Violet", new Color(0.36f, 0.2f, 0.75f), 0.26f, 0.6f, new Color(0.36f, 0.1f, 1f) * 1.9f);
            materials["green"] = MakeMaterial("Verdant Moss", new Color(0.075f, 0.27f, 0.19f), 0.04f, 0.22f);
            materials["leaf"] = MakeMaterial("Luminous Foliage", new Color(0.12f, 0.39f, 0.27f), 0.02f, 0.32f);
            materials["trunk"] = MakeMaterial("Weathered Bark", new Color(0.17f, 0.105f, 0.09f), 0.0f, 0.18f);
            materials["water"] = MakeMaterial("Still Aether", new Color(0.018f, 0.11f, 0.19f), 0.4f, 0.82f, new Color(0.01f, 0.08f, 0.19f) * 0.5f);
            materials["enemy"] = MakeMaterial("Rift Sentinel", new Color(0.29f, 0.045f, 0.16f), 0.18f, 0.45f, new Color(1f, 0.035f, 0.25f) * 1.3f);
            materials["player"] = MakeMaterial("Wanderer Ceramic", new Color(0.55f, 0.71f, 0.79f), 0.22f, 0.62f);
        }

        private Material MakeMaterial(string materialName, Color color, float metallic, float smoothness, Color? emission = null)
        {
            Shader shader = Shader.Find("Universal Render Pipeline/Lit");
            if (shader == null) shader = Shader.Find("Standard");
            if (shader == null) shader = Shader.Find("Diffuse");
            Material material = new Material(shader)
            {
                name = materialName,
                enableInstancing = true
            };
            generatedRuntimeAssets.Add(material);

            if (material.HasProperty("_BaseColor")) material.SetColor("_BaseColor", color);
            if (material.HasProperty("_Color")) material.SetColor("_Color", color);
            if (material.HasProperty("_Metallic")) material.SetFloat("_Metallic", metallic);
            if (material.HasProperty("_Smoothness")) material.SetFloat("_Smoothness", smoothness);
            if (emission.HasValue && material.HasProperty("_EmissionColor"))
            {
                material.SetColor("_EmissionColor", emission.Value);
                material.EnableKeyword("_EMISSION");
                material.globalIlluminationFlags = MaterialGlobalIlluminationFlags.RealtimeEmissive;
            }
            return material;
        }

        private void ConfigureAtmosphere()
        {
            RenderSettings.ambientMode = AmbientMode.Trilight;
            RenderSettings.ambientSkyColor = new Color(0.19f, 0.24f, 0.38f);
            RenderSettings.ambientEquatorColor = new Color(0.08f, 0.12f, 0.2f);
            RenderSettings.ambientGroundColor = new Color(0.025f, 0.035f, 0.055f);
            RenderSettings.reflectionIntensity = 0.7f;
            RenderSettings.fog = true;
            RenderSettings.fogMode = FogMode.Linear;
            RenderSettings.fogColor = new Color(0.035f, 0.055f, 0.11f);
            RenderSettings.fogStartDistance = 35f;
            RenderSettings.fogEndDistance = 180f;
            Shader skyShader = Shader.Find("Skybox/Procedural");
            if (skyShader != null)
            {
                Material sky = new Material(skyShader) { name = "Omnis Procedural Twilight Sky" };
                if (sky.HasProperty("_SkyTint")) sky.SetColor("_SkyTint", new Color(0.32f, 0.39f, 0.68f));
                if (sky.HasProperty("_GroundColor")) sky.SetColor("_GroundColor", new Color(0.035f, 0.045f, 0.09f));
                if (sky.HasProperty("_AtmosphereThickness")) sky.SetFloat("_AtmosphereThickness", 1.15f);
                if (sky.HasProperty("_Exposure")) sky.SetFloat("_Exposure", 0.72f);
                RenderSettings.skybox = sky;
                generatedRuntimeAssets.Add(sky);
            }

            GameObject lightObject = new GameObject("Nexus Key Light");
            lightObject.transform.SetParent(worldRoot, false);
            lightObject.transform.rotation = Quaternion.Euler(42f, -28f, 0f);
            Light key = lightObject.AddComponent<Light>();
            key.type = LightType.Directional;
            key.color = new Color(0.72f, 0.82f, 1f);
            key.intensity = 1.55f;
            key.shadows = LightShadows.Soft;

            CreatePointLight("Nexus Cyan Bounce", new Vector3(0f, 4.5f, 10f), new Color(0.08f, 0.8f, 1f), 7f, 25f);
            CreatePointLight("Rift Violet Bounce", new Vector3(-12f, 5f, 16f), new Color(0.4f, 0.12f, 1f), 4f, 16f);
            CreatePointLight("Aether Gold Bounce", new Vector3(12f, 3.5f, 4f), new Color(1f, 0.49f, 0.13f), 3f, 14f);

            GameObject volumeObject = new GameObject("Nexus Post Processing");
            volumeObject.transform.SetParent(worldRoot, false);
            Volume volume = volumeObject.AddComponent<Volume>();
            volume.isGlobal = true;
            volume.priority = 1f;
            VolumeProfile profile = ScriptableObject.CreateInstance<VolumeProfile>();
            profile.name = "Omnis Runtime Volume";
            generatedRuntimeAssets.Add(profile);
            Bloom bloom = profile.Add<Bloom>(true);
            bloom.threshold.overrideState = true;
            bloom.threshold.value = 0.88f;
            bloom.intensity.overrideState = true;
            bloom.intensity.value = 0.45f;
            bloom.scatter.overrideState = true;
            bloom.scatter.value = 0.65f;
            ColorAdjustments color = profile.Add<ColorAdjustments>(true);
            color.postExposure.overrideState = true;
            color.postExposure.value = -0.12f;
            color.contrast.overrideState = true;
            color.contrast.value = 16f;
            color.saturation.overrideState = true;
            color.saturation.value = 7f;
            Vignette vignette = profile.Add<Vignette>(true);
            vignette.intensity.overrideState = true;
            vignette.intensity.value = 0.22f;
            vignette.smoothness.overrideState = true;
            vignette.smoothness.value = 0.67f;
            vignette.color.overrideState = true;
            vignette.color.value = new Color(0.012f, 0.02f, 0.05f);
            volume.sharedProfile = profile;
        }

        private void CreatePointLight(string lightName, Vector3 position, Color color, float intensity, float range)
        {
            GameObject lightObject = new GameObject(lightName);
            lightObject.transform.SetParent(worldRoot, false);
            lightObject.transform.position = position;
            Light light = lightObject.AddComponent<Light>();
            light.type = LightType.Point;
            light.color = color;
            light.intensity = intensity;
            light.range = range;
            light.shadows = LightShadows.None;
        }

        private void BuildGroundAndPath()
        {
            GameObject ground = MakePrimitive("Aether Basin", PrimitiveType.Plane,
                new Vector3(0f, -0.22f, 5f), new Vector3(60f, 1f, 60f), materials["ground"], worldRoot);
            Texture2D soil = CreateProceduralTexture();
            Material groundMaterial = materials["ground"];
            if (groundMaterial.HasProperty("_BaseMap")) groundMaterial.SetTexture("_BaseMap", soil);
            if (groundMaterial.HasProperty("_MainTex")) groundMaterial.SetTexture("_MainTex", soil);
            if (groundMaterial.HasProperty("_BaseMap")) groundMaterial.SetTextureScale("_BaseMap", new Vector2(220f, 220f));
            if (groundMaterial.HasProperty("_MainTex")) groundMaterial.SetTextureScale("_MainTex", new Vector2(48f, 48f));

            // Distant mountain silhouettes frame the horizon without enclosing the playable frontier.
            for (int i = 0; i < 64; i++)
            {
                float angle = i * Mathf.PI * 2f / 64f;
                float radius = 228f + (float)random.NextDouble() * 26f;
                Vector3 p = new Vector3(Mathf.Cos(angle) * radius, 2f, Mathf.Sin(angle) * radius + 5f);
                GameObject ridge = MakePrimitive("Far Horizon Basalt Ridge", PrimitiveType.Sphere, p,
                    new Vector3(18f + (float)random.NextDouble() * 24f, 22f + (float)random.NextDouble() * 38f, 18f + (float)random.NextDouble() * 24f),
                    i % 4 == 0 ? materials["stoneLight"] : materials["stone"], worldRoot, false);
                ridge.transform.rotation = Quaternion.Euler((float)random.NextDouble() * 12f,
                    (float)random.NextDouble() * 360f, (float)random.NextDouble() * 12f);
            }

            // Segmented rune causeway leading from spawn to the central platform.
            for (int i = 0; i < 22; i++)
            {
                float z = -18f + i * 1.48f;
                Material m = i % 4 == 0 ? materials["cyan"] : (i % 2 == 0 ? materials["stoneLight"] : materials["stone"]);
                MakePrimitive("Nexus Causeway Slab", PrimitiveType.Cube,
                    new Vector3(0f, 0.025f, z), new Vector3(2.4f, 0.18f, 1.05f), m, worldRoot);
            }

            for (int side = -1; side <= 1; side += 2)
            {
                for (int i = 0; i < 9; i++)
                {
                    float z = -12f + i * 4.2f;
                    MakePrimitive("Wayfinder Pillar", PrimitiveType.Cylinder,
                        new Vector3(side * 3.3f, 0.7f, z), new Vector3(0.42f, 0.72f, 0.42f),
                        materials["stoneLight"], worldRoot);
                    MakePrimitive("Pillar Light", PrimitiveType.Sphere,
                        new Vector3(side * 3.3f, 1.55f, z), Vector3.one * 0.2f, materials["cyan"], worldRoot, false);
                }
            }
        }

        private Texture2D CreateProceduralTexture()
        {
            const int size = 512;
            Texture2D texture = new Texture2D(size, size, TextureFormat.RGBA32, true, false)
            {
                name = "Generated Aether Basalt Albedo",
                wrapMode = TextureWrapMode.Repeat,
                filterMode = FilterMode.Trilinear,
                anisoLevel = 8
            };
            Color[] pixels = new Color[size * size];
            Color dark = new Color(0.055f, 0.075f, 0.105f);
            Color light = new Color(0.18f, 0.22f, 0.28f);
            for (int y = 0; y < size; y++)
            {
                for (int x = 0; x < size; x++)
                {
                    float u = x / (float)size;
                    float v = y / (float)size;
                    float broad = Mathf.PerlinNoise(u * 5.5f, v * 5.5f);
                    float fine = Mathf.PerlinNoise(u * 31f + 7.3f, v * 31f + 2.8f);
                    float grain = Mathf.PerlinNoise(u * 113f, v * 113f);
                    float value = Mathf.Clamp01(broad * 0.58f + fine * 0.28f + grain * 0.14f);
                    pixels[y * size + x] = Color.Lerp(dark, light, value);
                }
            }
            texture.SetPixels(pixels);
            texture.Apply(true, true);
            generatedRuntimeAssets.Add(texture);
            return texture;
        }

        private void BuildNexus()
        {
            MakePrimitive("Outer Nexus Dais", PrimitiveType.Cylinder, new Vector3(0f, 0.02f, 10f),
                new Vector3(22f, 0.22f, 22f), materials["stone"], worldRoot);
            MakePrimitive("Nexus Inlaid Ring", PrimitiveType.Cylinder, new Vector3(0f, 0.18f, 10f),
                new Vector3(18.4f, 0.1f, 18.4f), materials["gold"], worldRoot);
            MakePrimitive("Inner Nexus Dais", PrimitiveType.Cylinder, new Vector3(0f, 0.29f, 10f),
                new Vector3(15.4f, 0.18f, 15.4f), materials["ground"], worldRoot);
            MakePrimitive("Nexus Inner Ring", PrimitiveType.Cylinder, new Vector3(0f, 0.4f, 10f),
                new Vector3(12.2f, 0.06f, 12.2f), materials["cyan"], worldRoot);
            MakePrimitive("Nexus Core Platform", PrimitiveType.Cylinder, new Vector3(0f, 0.48f, 10f),
                new Vector3(9.8f, 0.14f, 9.8f), materials["stoneLight"], worldRoot);

            // Layered faceted obelisk, metallic ribs, and emissive core.
            Transform obelisk = MakePrimitive("Obelisk - Interactive Nexus", PrimitiveType.Cube,
                new Vector3(0f, 3.25f, 10f), new Vector3(1.65f, 5.2f, 1.65f), materials["stoneLight"], worldRoot).transform;
            obelisk.rotation = Quaternion.Euler(0f, 45f, 0f);
            MakePrimitive("Obelisk Lower Collar", PrimitiveType.Cylinder, new Vector3(0f, 0.95f, 10f),
                new Vector3(2.65f, 0.28f, 2.65f), materials["gold"], worldRoot);
            MakePrimitive("Obelisk Upper Collar", PrimitiveType.Cylinder, new Vector3(0f, 5.15f, 10f),
                new Vector3(2.0f, 0.16f, 2.0f), materials["gold"], worldRoot);
            GameObject core = MakePrimitive("Living Nexus Core", PrimitiveType.Sphere,
                new Vector3(0f, 6.4f, 10f), Vector3.one * 1.25f, materials["cyan"], worldRoot, false);
            core.AddComponent<OmnisAmbientMotion>();
            for (int i = 0; i < 4; i++)
            {
                float angle = i * 90f * Mathf.Deg2Rad;
                Vector3 pos = new Vector3(Mathf.Cos(angle) * 2.15f, 3.6f, 10f + Mathf.Sin(angle) * 2.15f);
                GameObject shard = MakePrimitive("Orbiting Nexus Shard", PrimitiveType.Cube, pos,
                    new Vector3(0.28f, 1.05f, 0.28f), i % 2 == 0 ? materials["cyan"] : materials["violet"], worldRoot, false);
                shard.transform.rotation = Quaternion.Euler(0f, i * 45f, 25f);
                shard.AddComponent<OmnisAmbientMotion>();
            }

            CapsuleCollider collider = obelisk.gameObject.AddComponent<CapsuleCollider>();
            collider.center = new Vector3(0f, 0f, 0f);
            collider.height = 5.2f;
            collider.radius = 0.9f;
            OmnisInteractable interactable = obelisk.gameObject.AddComponent<OmnisInteractable>();
            interactable.Configure("Omnis.Nexus.Obelisk", "Obelisco del Nexo", 100, true, 3);

            // Ruined ring arches frame the core without blocking the walkable platform.
            for (int side = -1; side <= 1; side += 2)
            {
                float x = side * 7.8f;
                MakePrimitive("Nexus Gate Column", PrimitiveType.Cube, new Vector3(x, 2.6f, 10f),
                    new Vector3(0.75f, 5f, 0.95f), materials["stoneLight"], worldRoot);
                MakePrimitive("Nexus Gate Cap", PrimitiveType.Cube, new Vector3(x, 5.15f, 10f),
                    new Vector3(2.0f, 0.36f, 1.1f), materials["gold"], worldRoot);
            }
        }

        private void BuildRuins()
        {
            for (int i = 0; i < 7; i++)
            {
                float angle = (i / 7f) * Mathf.PI * 2f + 0.3f;
                float radius = 27f + (float)random.NextDouble() * 5f;
                Vector3 center = new Vector3(Mathf.Cos(angle) * radius, 0f, 10f + Mathf.Sin(angle) * radius);
                BuildRuin(center, 2.2f + (float)random.NextDouble() * 1.7f, (float)random.NextDouble() * 360f);
            }
        }

        private void BuildRuin(Vector3 center, float size, float yaw)
        {
            Transform ruin = new GameObject("Ancient Ruin").transform;
            ruin.SetParent(worldRoot, false);
            ruin.position = center;
            ruin.rotation = Quaternion.Euler(0f, yaw, 0f);
            for (int side = -1; side <= 1; side += 2)
            {
                GameObject pillar = MakePrimitive("Fractured Column", PrimitiveType.Cylinder,
                    Vector3.zero, new Vector3(size * 0.18f, size * 1.05f, size * 0.18f),
                    materials["stoneLight"], ruin);
                pillar.transform.localPosition = new Vector3(side * size, size * 1.05f, 0f);
                pillar.transform.rotation = Quaternion.Euler(0f, 0f, side * (float)random.NextDouble() * 7f);
                GameObject capital = MakePrimitive("Column Capital", PrimitiveType.Cube,
                    Vector3.zero, new Vector3(size * 0.45f, size * 0.2f, size * 0.45f),
                    materials["gold"], ruin);
                capital.transform.localPosition = new Vector3(side * size, size * 2.13f, 0f);
            }
            GameObject lintel = MakePrimitive("Broken Lintel", PrimitiveType.Cube, Vector3.zero,
                new Vector3(size * 2.45f, size * 0.23f, size * 0.36f), materials["stone"], ruin);
            lintel.transform.localPosition = new Vector3(0f, size * 2.12f, 0f);
            GameObject plinth = MakePrimitive("Ruin Plinth", PrimitiveType.Cube, Vector3.zero,
                new Vector3(size * 3.0f, 0.3f, size * 1.2f), materials["stone"], ruin);
            plinth.transform.localPosition = new Vector3(0f, 0.12f, 0f);
            GameObject inscription = MakePrimitive("Rift Inscription", PrimitiveType.Cube, Vector3.zero,
                new Vector3(size * 0.55f, size * 0.6f, 0.08f), materials["violet"], ruin, false);
            inscription.transform.localPosition = new Vector3(0f, size * 0.8f, size * 0.23f);
        }

        private void BuildNature()
        {
            for (int i = 0; i < 38; i++)
            {
                float x = -46f + (float)random.NextDouble() * 92f;
                float z = -30f + (float)random.NextDouble() * 82f;
                if (Mathf.Abs(x) < 13f && z > -19f && z < 23f) continue;
                BuildTree(new Vector3(x, 0f, z), 1.8f + (float)random.NextDouble() * 2.5f);
            }

            for (int i = 0; i < 74; i++)
            {
                float x = -47f + (float)random.NextDouble() * 94f;
                float z = -27f + (float)random.NextDouble() * 80f;
                if (Mathf.Abs(x) < 11f && z > -18f && z < 22f) continue;
                float scale = 0.4f + (float)random.NextDouble() * 1.7f;
                GameObject rock = MakePrimitive("Weathered Aether Rock", PrimitiveType.Sphere,
                    new Vector3(x, scale * 0.28f, z), new Vector3(scale * 1.3f, scale * 0.65f, scale),
                    i % 5 == 0 ? materials["stoneLight"] : materials["stone"], worldRoot);
                rock.transform.rotation = Quaternion.Euler((float)random.NextDouble() * 18f,
                    (float)random.NextDouble() * 360f, (float)random.NextDouble() * 18f);
            }

            // A few pools add a cool reflective note along the perimeter.
            for (int i = 0; i < 4; i++)
            {
                float z = -10f + i * 18f;
                MakePrimitive("Aether Pool", PrimitiveType.Cylinder, new Vector3((i % 2 == 0 ? -1f : 1f) * 36f, -0.01f, z),
                    new Vector3(5.5f, 0.06f, 8f), materials["water"], worldRoot);
            }
        }

        // First bounded open-world slice: long trails, a settlement, crossings and distant landmarks.
        private void BuildOpenWorldExpansion()
        {
            BuildTrail(new Vector3(0f, 0.02f, 20f), new Vector3(105f, 0.02f, 88f), 46, 2.8f, "Verdant Trail");
            BuildTrail(new Vector3(-5f, 0.02f, 20f), new Vector3(-118f, 0.02f, 104f), 52, 3.1f, "Shattered Ridge Trail");
            BuildTrail(new Vector3(0f, 0.02f, -12f), new Vector3(88f, 0.02f, -112f), 45, 2.6f, "Aether Marsh Trail");

            for (int i = 0; i < 18; i++)
            {
                float z = -48f + i * 9f;
                float x = 58f + Mathf.Sin(i * 0.42f) * 13f;
                GameObject water = MakePrimitive("Aether River Segment", PrimitiveType.Cube,
                    new Vector3(x, -0.075f, z), new Vector3(13f, 0.08f, 10f), materials["water"], worldRoot, false);
                water.transform.rotation = Quaternion.Euler(0f, Mathf.Sin(i * 0.42f) * 18f, 0f);
            }
            BuildBridge(new Vector3(64f, 0.1f, 25f), 18f, "Old Pilgrim Bridge");
            BuildBridge(new Vector3(66f, 0.1f, -42f), 18f, "Mosslight Crossing");

            MakePrimitive("Wayfarer Village Plaza", PrimitiveType.Cylinder, new Vector3(112f, 0.025f, 88f),
                new Vector3(24f, 0.16f, 24f), materials["stoneLight"], worldRoot);
            for (int i = 0; i < 8; i++)
            {
                float a = i * Mathf.PI * 2f / 8f;
                BuildVillageHouse(new Vector3(112f + Mathf.Cos(a) * 20f, 0f, 88f + Mathf.Sin(a) * 20f), i);
            }
            MakePrimitive("Village Beacon Pedestal", PrimitiveType.Cylinder, new Vector3(112f, 0.45f, 88f),
                new Vector3(2.8f, 0.8f, 2.8f), materials["gold"], worldRoot);
            GameObject beacon = MakePrimitive("Wayfarer Beacon", PrimitiveType.Sphere, new Vector3(112f, 2.1f, 88f),
                Vector3.one * 1.1f, materials["cyan"], worldRoot, false);
            beacon.AddComponent<OmnisAmbientMotion>();
            CreatePointLight("Wayfarer Beacon Light", new Vector3(112f, 2.2f, 88f), new Color(0.12f, 0.8f, 1f), 3f, 18f);

            BuildElderTree(new Vector3(-118f, 0f, 104f), 13f);
            BuildWatchtower(new Vector3(-158f, 0f, 124f));
            BuildMarshMonolith(new Vector3(88f, 0f, -112f));

            // Stable seeded placement keeps the same world landmarks across launches.
            for (int i = 0; i < 190; i++)
            {
                float x = -205f + (float)random.NextDouble() * 410f;
                float z = -195f + (float)random.NextDouble() * 390f;
                if (Mathf.Abs(x) < 18f && z > -24f && z < 30f) continue;
                if (Vector2.Distance(new Vector2(x, z), new Vector2(112f, 88f)) < 33f) continue;
                if (Vector2.Distance(new Vector2(x, z), new Vector2(-118f, 104f)) < 22f) continue;
                if (Mathf.Abs(x - 62f) < 18f && z > -55f && z < 120f) continue;
                BuildTree(new Vector3(x, 0f, z), 2.1f + (float)random.NextDouble() * 4.8f);
            }
            for (int i = 0; i < 150; i++)
            {
                float x = -215f + (float)random.NextDouble() * 430f;
                float z = -205f + (float)random.NextDouble() * 410f;
                if (Mathf.Abs(x) < 14f && z > -20f && z < 25f) continue;
                float s = 0.8f + (float)random.NextDouble() * 3.4f;
                GameObject rock = MakePrimitive("Frontier Weathered Stone", PrimitiveType.Sphere,
                    new Vector3(x, s * 0.25f, z), new Vector3(s * 1.4f, s * 0.7f, s),
                    i % 6 == 0 ? materials["stoneLight"] : materials["stone"], worldRoot, false);
                rock.transform.rotation = Quaternion.Euler((float)random.NextDouble() * 20f,
                    (float)random.NextDouble() * 360f, (float)random.NextDouble() * 20f);
            }
            BuildSkySpire(new Vector3(176f, 0f, -158f));
        }

        private void BuildTrail(Vector3 start, Vector3 end, int segments, float width, string trailName)
        {
            float yaw = Mathf.Atan2(end.x - start.x, end.z - start.z) * Mathf.Rad2Deg;
            for (int i = 0; i < segments; i++)
            {
                float t = (i + 0.5f) / segments;
                Vector3 point = Vector3.Lerp(start, end, t);
                point.x += Mathf.Sin(t * Mathf.PI * 4f) * 1.7f;
                Material m = i % 7 == 0 ? materials["cyan"] : (i % 2 == 0 ? materials["stoneLight"] : materials["stone"]);
                GameObject slab = MakePrimitive(trailName + " Rune Slab", PrimitiveType.Cube, point,
                    new Vector3(width, 0.16f, Vector3.Distance(start, end) / segments * 0.92f), m, worldRoot);
                slab.transform.rotation = Quaternion.Euler(0f, yaw, 0f);
            }
        }

        private void BuildBridge(Vector3 center, float length, string bridgeName)
        {
            GameObject deck = MakePrimitive(bridgeName + " Deck", PrimitiveType.Cube, center,
                new Vector3(5.5f, 0.55f, length), materials["stoneLight"], worldRoot);
            deck.transform.rotation = Quaternion.Euler(0f, -12f, 0f);
            for (int side = -1; side <= 1; side += 2)
                for (int i = 0; i < 7; i++)
                {
                    Vector3 post = center + new Vector3(side * 2.45f, 0.95f, -length * 0.43f + i * length * 0.143f);
                    MakePrimitive(bridgeName + " Rail Post", PrimitiveType.Cylinder, post,
                        new Vector3(0.22f, 0.7f, 0.22f), materials["gold"], worldRoot);
                }
        }

        private void BuildVillageHouse(Vector3 center, int variant)
        {
            float width = 5.5f + (variant % 3) * 0.8f;
            Material wall = variant % 2 == 0 ? materials["stoneLight"] : materials["stone"];
            MakePrimitive("Wayfarer House Walls", PrimitiveType.Cube, center + Vector3.up * 1.7f,
                new Vector3(width, 3.4f, width * 0.82f), wall, worldRoot);
            GameObject roof = MakePrimitive("Wayfarer House Roof", PrimitiveType.Cube, center + Vector3.up * 3.75f,
                new Vector3(width * 1.22f, 0.75f, width), materials["trunk"], worldRoot);
            roof.transform.rotation = Quaternion.Euler(0f, variant * 22.5f, 8f * (variant % 3 - 1));
            MakePrimitive("Wayfarer House Door", PrimitiveType.Cube, center + new Vector3(0f, 0.95f, -width * 0.415f),
                new Vector3(1.05f, 1.9f, 0.12f), materials["gold"], worldRoot, false);
            MakePrimitive("Wayfarer House Window", PrimitiveType.Cube, center + new Vector3(width * 0.28f, 2.05f, -width * 0.43f),
                new Vector3(0.72f, 0.72f, 0.13f), materials["cyan"], worldRoot, false);
        }

        private void BuildElderTree(Vector3 center, float size)
        {
            MakePrimitive("Elder Tree Trunk", PrimitiveType.Cylinder, center + Vector3.up * size * 0.78f,
                new Vector3(size * 0.32f, size * 0.82f, size * 0.32f), materials["trunk"], worldRoot);
            for (int i = 0; i < 6; i++)
            {
                float a = i * Mathf.PI / 3f;
                Vector3 limb = center + new Vector3(Mathf.Cos(a) * size * 0.45f, size * 1.18f, Mathf.Sin(a) * size * 0.45f);
                GameObject branch = MakePrimitive("Elder Tree Ancient Limb", PrimitiveType.Cylinder, limb,
                    new Vector3(size * 0.11f, size * 0.72f, size * 0.11f), materials["trunk"], worldRoot, false);
                branch.transform.rotation = Quaternion.Euler(Mathf.Sin(a) * 30f, 0f, Mathf.Cos(a) * 30f);
                GameObject canopy = MakePrimitive("Elder Tree Crown", PrimitiveType.Sphere,
                    center + new Vector3(Mathf.Cos(a) * size * 0.42f, size * 2.05f, Mathf.Sin(a) * size * 0.42f),
                    new Vector3(size * 0.88f, size * 0.72f, size * 0.88f), materials["leaf"], worldRoot, false);
                canopy.AddComponent<OmnisAmbientMotion>();
            }
            CreatePointLight("Elder Tree Heartlight", center + Vector3.up * size * 1.75f, new Color(0.15f, 0.9f, 0.58f), 2.4f, 22f);
        }

        private void BuildWatchtower(Vector3 center)
        {
            MakePrimitive("Broken Watchtower Base", PrimitiveType.Cylinder, center + Vector3.up * 0.35f,
                new Vector3(8f, 0.7f, 8f), materials["stone"], worldRoot);
            MakePrimitive("Broken Watchtower Shaft", PrimitiveType.Cylinder, center + Vector3.up * 7f,
                new Vector3(3.4f, 7f, 3.4f), materials["stoneLight"], worldRoot);
            MakePrimitive("Broken Watchtower Crown", PrimitiveType.Cube, center + Vector3.up * 13.5f,
                new Vector3(8f, 1f, 8f), materials["gold"], worldRoot);
            GameObject flame = MakePrimitive("Watchtower Rift Beacon", PrimitiveType.Sphere,
                center + Vector3.up * 15f, Vector3.one * 1.5f, materials["violet"], worldRoot, false);
            flame.AddComponent<OmnisAmbientMotion>();
            CreatePointLight("Watchtower Beacon Light", center + Vector3.up * 15f, new Color(0.48f, 0.12f, 1f), 3.5f, 28f);
        }

        private void BuildMarshMonolith(Vector3 center)
        {
            MakePrimitive("Marsh Monolith Pedestal", PrimitiveType.Cylinder, center + Vector3.up * 0.25f,
                new Vector3(7f, 0.5f, 7f), materials["stone"], worldRoot);
            GameObject monolith = MakePrimitive("Marsh Rift Monolith", PrimitiveType.Cube,
                center + Vector3.up * 6f, new Vector3(2.4f, 12f, 2.4f), materials["violet"], worldRoot, false);
            monolith.transform.rotation = Quaternion.Euler(0f, 23f, 4f);
            CreatePointLight("Marsh Monolith Glow", center + Vector3.up * 5f, new Color(0.5f, 0.12f, 1f), 2.6f, 20f);
        }

        private void BuildSkySpire(Vector3 center)
        {
            MakePrimitive("Far Sky Spire Base", PrimitiveType.Cylinder, center + Vector3.up * 0.5f,
                new Vector3(14f, 1f, 14f), materials["stone"], worldRoot, false);
            for (int i = 0; i < 7; i++)
            {
                float y = 5f + i * 5f;
                GameObject segment = MakePrimitive("Far Sky Spire Segment", PrimitiveType.Cube,
                    center + Vector3.up * y, new Vector3(3.2f - i * 0.25f, 4.5f, 3.2f - i * 0.25f),
                    i % 2 == 0 ? materials["cyan"] : materials["stoneLight"], worldRoot, false);
                segment.transform.rotation = Quaternion.Euler(0f, i * 13f, i % 2 == 0 ? 4f : -4f);
            }
            CreatePointLight("Far Sky Spire Beacon", center + Vector3.up * 37f, new Color(0.12f, 0.8f, 1f), 2.4f, 32f);
        }

        private void BuildTree(Vector3 position, float size)
        {
            MakePrimitive("Ancient Tree Trunk", PrimitiveType.Cylinder,
                position + Vector3.up * size * 0.65f, new Vector3(size * 0.11f, size * 0.7f, size * 0.11f),
                materials["trunk"], worldRoot);

            for (int i = 0; i < 3; i++)
            {
                float layerSize = size * (0.72f - i * 0.13f);
                Vector3 leafPosition = position + Vector3.up * size * (1.15f + i * 0.48f);
                GameObject leaves = MakePrimitive("Aether Canopy", PrimitiveType.Sphere, leafPosition,
                    new Vector3(layerSize, layerSize * 0.76f, layerSize), i == 1 ? materials["green"] : materials["leaf"], worldRoot, false);
                leaves.transform.rotation = Quaternion.Euler(0f, (float)random.NextDouble() * 360f, 0f);
            }
        }

        private void BuildRelicsAndSentinels()
        {
            CreateRelic("Omnis.Relic.A", "Fragmento de Memoria I", new Vector3(-7f, 1.25f, 5f), materials["cyan"]);
            CreateRelic("Omnis.Relic.B", "Fragmento de Memoria II", new Vector3(7f, 1.25f, 7f), materials["violet"]);
            CreateRelic("Omnis.Relic.C", "Fragmento de Memoria III", new Vector3(0f, 1.25f, 19f), materials["gold"]);

            CreateSentinel("Sentinel-East", new Vector3(16f, 1.15f, 19f));
            CreateSentinel("Sentinel-West", new Vector3(-16f, 1.15f, 18f));
            CreateSentinel("Sentinel-North", new Vector3(0f, 1.15f, 34f));
        }

        private void CreateRelic(string id, string label, Vector3 position, Material material)
        {
            GameObject relic = new GameObject(label);
            relic.transform.SetParent(worldRoot, false);
            relic.transform.position = position;
            SphereCollider collider = relic.AddComponent<SphereCollider>();
            collider.radius = 0.9f;
            OmnisInteractable interactable = relic.AddComponent<OmnisInteractable>();
            interactable.Configure(id, label, 50, false);

            Mesh mesh = CreateCrystalMesh();
            GameObject crystal = new GameObject("Faceted Crystal");
            crystal.transform.SetParent(relic.transform, false);
            crystal.transform.localPosition = Vector3.zero;
            crystal.transform.localScale = new Vector3(0.74f, 1.1f, 0.74f);
            MeshFilter filter = crystal.AddComponent<MeshFilter>();
            filter.sharedMesh = mesh;
            MeshRenderer renderer = crystal.AddComponent<MeshRenderer>();
            renderer.sharedMaterial = material;

            GameObject ring = MakePrimitive("Relic Halo", PrimitiveType.Cylinder,
                position + Vector3.down * 0.68f, new Vector3(1.6f, 0.045f, 1.6f), material, worldRoot, false);
            relic.AddComponent<OmnisAmbientMotion>();
            CreatePointLight(label + " Light", position + Vector3.up * 0.2f, material.color, 2.1f, 7f);
            if (OmnisGameState.Instance != null && OmnisGameState.Instance.WasInteractionCompleted(id))
                relic.SetActive(false);
        }

        private Mesh CreateCrystalMesh()
        {
            Mesh mesh = new Mesh { name = "Procedural Faceted Memory Crystal" };
            Vector3[] vertices =
            {
                new Vector3(0f, 1.2f, 0f),
                new Vector3(0.78f, 0f, 0f),
                new Vector3(0f, 0f, 0.78f),
                new Vector3(-0.78f, 0f, 0f),
                new Vector3(0f, 0f, -0.78f),
                new Vector3(0f, -0.85f, 0f)
            };
            int[] triangles =
            {
                0,2,1, 0,3,2, 0,4,3, 0,1,4,
                5,1,2, 5,2,3, 5,3,4, 5,4,1
            };
            mesh.vertices = vertices;
            mesh.triangles = triangles;
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
            generatedRuntimeAssets.Add(mesh);
            return mesh;
        }

        private void CreateSentinel(string id, Vector3 position)
        {
            if (OmnisGameState.Instance != null && OmnisGameState.Instance.WasSentinelDefeated(id))
                return;

            GameObject enemy = MakePrimitive(id, PrimitiveType.Sphere, position,
                new Vector3(1.25f, 1.5f, 1.25f), materials["enemy"], worldRoot);
            enemy.AddComponent<OmnisSentinel>().Configure(id, 100);
            GameObject core = MakePrimitive("Sentinel Core", PrimitiveType.Sphere,
                position + Vector3.up * 0.08f, Vector3.one * 0.57f, materials["violet"], enemy.transform, false);
            core.transform.localPosition = new Vector3(0f, 0.08f, 0f);
        }

        private void BuildPlayer()
        {
            GameObject playerObject = new GameObject("Omnis Wanderer");
            playerObject.transform.SetParent(worldRoot, false);
            playerObject.transform.position = new Vector3(0f, 1.05f, -22f);
            CharacterController controller = playerObject.AddComponent<CharacterController>();
            controller.height = 2f;
            controller.radius = 0.38f;
            controller.center = Vector3.zero;
            controller.slopeLimit = 48f;
            controller.stepOffset = 0.35f;

            GameObject body = MakePrimitive("Wanderer Body", PrimitiveType.Capsule,
                playerObject.transform.position, new Vector3(0.78f, 0.96f, 0.78f), materials["player"], playerObject.transform, false);
            body.transform.localPosition = Vector3.zero;
            MakePrimitive("Wanderer Aether Core", PrimitiveType.Sphere,
                playerObject.transform.position + Vector3.up * 0.28f,
                Vector3.one * 0.22f, materials["cyan"], playerObject.transform, false).transform.localPosition = new Vector3(0f, 0.28f, 0.37f);
            MakePrimitive("Wanderer Shoulder Guard", PrimitiveType.Sphere,
                playerObject.transform.position + Vector3.up * 0.53f,
                new Vector3(0.28f, 0.23f, 0.34f), materials["gold"], playerObject.transform, false).transform.localPosition = new Vector3(0.28f, 0.53f, 0f);

            GameObject pivot = new GameObject("Camera Pivot");
            pivot.transform.SetParent(playerObject.transform, false);
            pivot.transform.localPosition = new Vector3(0f, 0.55f, 0f);

            GameObject cameraObject = new GameObject("Main Camera");
            cameraObject.tag = "MainCamera";
            cameraObject.transform.SetParent(pivot.transform, false);
            cameraObject.transform.localPosition = new Vector3(0f, 1.15f, -6.5f);
            cameraObject.transform.localRotation = Quaternion.Euler(8f, 0f, 0f);
            Camera camera = cameraObject.AddComponent<Camera>();
            camera.fieldOfView = 62f;
            camera.nearClipPlane = 0.08f;
            camera.farClipPlane = 400f;
            camera.allowHDR = true;
            camera.allowMSAA = true;
            camera.backgroundColor = new Color(0.025f, 0.045f, 0.09f);
            cameraObject.AddComponent<AudioListener>();
            UniversalAdditionalCameraData cameraData = cameraObject.AddComponent<UniversalAdditionalCameraData>();
            cameraData.renderPostProcessing = true;
            cameraData.antialiasing = AntialiasingMode.FastApproximateAntialiasing;
            cameraData.antialiasingQuality = AntialiasingQuality.High;

            OmnisPlayerController playerController = playerObject.AddComponent<OmnisPlayerController>();
            playerObject.AddComponent<OmnisResonanceCombat>();
            playerController.ConfigureCamera(pivot.transform, camera);

            GameObject hudObject = new GameObject("Omnis HUD");
            hudObject.transform.SetParent(worldRoot, false);
            hudObject.AddComponent<OmnisHud>().Configure(playerController);
        }

        private void BuildAmbientParticles()
        {
            GameObject particlesObject = new GameObject("Nexus Aether Motes");
            particlesObject.transform.SetParent(worldRoot, false);
            particlesObject.transform.position = new Vector3(0f, 3.3f, 10f);
            ParticleSystem system = particlesObject.AddComponent<ParticleSystem>();
            var main = system.main;
            main.loop = true;
            main.startLifetime = 9f;
            main.startSpeed = 0.16f;
            main.startSize = new ParticleSystem.MinMaxCurve(0.035f, 0.12f);
            main.startColor = new ParticleSystem.MinMaxGradient(new Color(0.3f, 0.95f, 1f, 0.72f), new Color(0.62f, 0.31f, 1f, 0.8f));
            main.maxParticles = 280;
            var emission = system.emission;
            emission.rateOverTime = 24f;
            var shape = system.shape;
            shape.shapeType = ParticleSystemShapeType.Sphere;
            shape.radius = 17f;
            shape.radiusThickness = 0.15f;
            var velocity = system.velocityOverLifetime;
            velocity.enabled = true;
            velocity.y = new ParticleSystem.MinMaxCurve(0.08f, 0.24f);
            var renderer = particlesObject.GetComponent<ParticleSystemRenderer>();
            Shader particleShader = Shader.Find("Universal Render Pipeline/Particles/Unlit");
            if (particleShader == null) particleShader = Shader.Find("Particles/Standard Unlit");
            if (particleShader != null)
            {
                Material particleMaterial = new Material(particleShader) { name = "Aether Mote Material" };
                generatedRuntimeAssets.Add(particleMaterial);
                renderer.sharedMaterial = particleMaterial;
            }
            system.Play();
        }

        private void OnDestroy()
        {
            if (worldRoot != null) Destroy(worldRoot.gameObject);
            foreach (UnityEngine.Object generated in generatedRuntimeAssets)
            {
                if (generated != null) Destroy(generated);
            }
            generatedRuntimeAssets.Clear();
            materials.Clear();
        }

        private GameObject MakePrimitive(string objectName, PrimitiveType type, Vector3 position,
            Vector3 scale, Material material, Transform parent, bool keepCollider = true)
        {
            GameObject go = GameObject.CreatePrimitive(type);
            go.name = objectName;
            go.transform.SetParent(parent, true);
            go.transform.position = position;
            go.transform.localScale = scale;
            Renderer renderer = go.GetComponent<Renderer>();
            if (renderer != null && material != null) renderer.sharedMaterial = material;
            if (!keepCollider)
            {
                Collider collider = go.GetComponent<Collider>();
                if (collider != null) Destroy(collider);
            }
            return go;
        }
    }
}
