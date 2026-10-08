using System.Collections.Generic;
using UnityEngine;

namespace Omnis
{
    /// <summary>
    /// Premium visual vertical-slice world built from deterministic runtime geometry.
    /// This is deliberately asset-free: it never pretends Unreal .uasset/.umap files are Unity assets.
    /// Transferable source meshes/textures can be attached later without changing the world contract.
    /// </summary>
    public static class OmnisPremiumWorld
    {
        const string RootName = "OMNIS_PREMIUM_WORLD";

        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
        static void Build()
        {
            if (GameObject.Find(RootName) != null) return;

            var root = new GameObject(RootName);
            var world = root.AddComponent<PremiumWorldRuntime>();
            world.Build();
        }
    }

    public sealed class PremiumWorldRuntime : MonoBehaviour
    {
        readonly List<Material> materials = new();
        Transform architecture;
        Transform nature;
        Transform landmarks;

        public void Build()
        {
            architecture = Child("Architecture");
            nature = Child("Nature");
            landmarks = Child("Landmarks");

            ConfigureLighting();
            BuildGround();
            BuildRoadGrid();
            BuildDistricts();
            BuildNexusPlaza();
            BuildCycleObelisks();
            BuildNature();
            BuildWater();
        }

        Transform Child(string name)
        {
            var go = new GameObject(name);
            go.transform.SetParent(transform, false);
            return go.transform;
        }

        Material Mat(string name, Color color, float metallic = 0f, float smoothness = .5f, float emission = 0f)
        {
            var shader = Shader.Find("Standard") ?? Shader.Find("Universal Render Pipeline/Lit");
            var m = new Material(shader) { name = name };
            m.color = color;
            if (m.HasProperty("_Metallic")) m.SetFloat("_Metallic", metallic);
            if (m.HasProperty("_Glossiness")) m.SetFloat("_Glossiness", smoothness);
            if (emission > 0f && m.HasProperty("_EmissionColor"))
            {
                m.EnableKeyword("_EMISSION");
                m.SetColor("_EmissionColor", color * emission);
            }
            materials.Add(m);
            return m;
        }

        void ConfigureLighting()
        {
            RenderSettings.ambientMode = UnityEngine.Rendering.AmbientMode.Trilight;
            RenderSettings.ambientSkyColor = new Color(.08f, .13f, .24f);
            RenderSettings.ambientEquatorColor = new Color(.18f, .22f, .30f);
            RenderSettings.ambientGroundColor = new Color(.025f, .035f, .055f);
            RenderSettings.fog = true;
            RenderSettings.fogColor = new Color(.055f, .08f, .14f);
            RenderSettings.fogDensity = .006f;

            var light = FindFirstObjectByType<Light>();
            if (light == null)
            {
                var go = new GameObject("Omnis_Sun");
                light = go.AddComponent<Light>();
                light.type = LightType.Directional;
            }
            light.intensity = 1.15f;
            light.color = new Color(1f, .91f, .78f);
            light.transform.rotation = Quaternion.Euler(38f, -32f, 0f);
        }

        void BuildGround()
        {
            var ground = GameObject.CreatePrimitive(PrimitiveType.Plane);
            ground.name = "Omnis_Terrain";
            ground.transform.SetParent(transform, false);
            ground.transform.localScale = new Vector3(12f, 1f, 12f);
            ground.GetComponent<Renderer>().sharedMaterial = Mat("Terrain", new Color(.055f, .085f, .095f), 0f, .35f);
        }

        void BuildRoadGrid()
        {
            var road = Mat("Road", new Color(.035f, .045f, .065f), .15f, .7f);
            var lane = Mat("LaneMark", new Color(.95f, .76f, .28f), .1f, .55f, .15f);

            for (int i = -4; i <= 4; i++)
            {
                Cube("Road_EW_" + i, architecture, new Vector3(0, .025f, i * 18f), new Vector3(180f, .05f, 7f), road);
                Cube("Road_NS_" + i, architecture, new Vector3(i * 18f, .026f, 0), new Vector3(7f, .05f, 180f), road);
                Cube("Lane_EW_" + i, architecture, new Vector3(0, .065f, i * 18f), new Vector3(150f, .025f, .12f), lane);
                Cube("Lane_NS_" + i, architecture, new Vector3(i * 18f, .066f, 0), new Vector3(.12f, .025f, 150f), lane);
            }
        }

        void BuildDistricts()
        {
            var glass = Mat("SkyGlass", new Color(.08f, .34f, .46f), .45f, .85f);
            var stone = Mat("ArchitectureStone", new Color(.20f, .25f, .31f), .1f, .55f);
            var accent = Mat("OmnisAccent", new Color(.08f, .85f, 1f), .2f, .75f, 2.2f);
            var warm = Mat("WarmAccent", new Color(1f, .38f, .12f), .1f, .7f, 1.6f);

            int index = 0;
            for (int x = -3; x <= 3; x++)
            for (int z = -3; z <= 3; z++)
            {
                if (Mathf.Abs(x) <= 1 && Mathf.Abs(z) <= 1) continue;

                float h = 8f + ((Mathf.Abs(x * 17 + z * 31) % 13) * 1.7f);
                float w = 8f + ((Mathf.Abs(x * 11 - z * 7) % 4) * 1.5f);
                Vector3 p = new Vector3(x * 18f + (z % 2) * 2f, h * .5f, z * 18f + (x % 2) * 2f);
                var b = Cube("District_Building_" + index++, architecture, p, new Vector3(w, h, w), (index % 3 == 0) ? glass : stone);

                for (int floor = 1; floor < Mathf.FloorToInt(h / 3f); floor++)
                {
                    float side = w + .03f;
                    Cube("WindowBand", b.transform, new Vector3(0, floor * 3f - h * .5f, side * .51f), new Vector3(w * .72f, .45f, .06f), (floor % 2 == 0) ? accent : warm);
                }
                if (index % 4 == 0)
                    Cube("RoofBeacon", b.transform, new Vector3(0, h * .55f, 0), new Vector3(.5f, 2.5f, .5f), accent);
            }
        }

        void BuildNexusPlaza()
        {
            var plaza = Mat("NexusPlaza", new Color(.12f, .16f, .22f), .25f, .8f);
            var cyan = Mat("NexusCyan", new Color(.05f, .8f, 1f), .15f, .75f, 3f);
            Cylinder("NexusPlatform", landmarks, new Vector3(0, .25f, 0), new Vector3(18f, .5f, 18f), plaza);
            Cylinder("NexusRing", landmarks, new Vector3(0, .58f, 0), new Vector3(12f, .12f, 12f), cyan);

            for (int i = 0; i < 8; i++)
            {
                float a = i * Mathf.PI * 2f / 8f;
                Vector3 p = new Vector3(Mathf.Cos(a) * 8f, 1.2f, Mathf.Sin(a) * 8f);
                Cube("NexusPillar_" + i, landmarks, p, new Vector3(1.1f, 2.4f, 1.1f), cyan);
            }
        }

        void BuildCycleObelisks()
        {
            var obelisk = Mat("CycleObelisk", new Color(.16f, .08f, .25f), .35f, .8f);
            var glow = Mat("CycleGlow", new Color(.7f, .12f, 1f), .2f, .65f, 3.5f);

            Vector3[] positions =
            {
                new(-62f, 2f, -62f), new(62f, 2f, -62f),
                new(-62f, 2f, 62f), new(62f, 2f, 62f)
            };
            for (int i = 0; i < positions.Length; i++)
            {
                Cylinder("CycleObelisk_" + i, landmarks, positions[i], new Vector3(2.2f, 8f, 2.2f), obelisk);
                Cube("CycleCore_" + i, landmarks, positions[i] + Vector3.up * 6f, new Vector3(.8f, 4f, .8f), glow);
            }
        }

        void BuildNature()
        {
            var trunk = Mat("TreeTrunk", new Color(.20f, .12f, .07f), 0f, .4f);
            var leaf = Mat("Canopy", new Color(.08f, .42f, .22f), 0f, .45f);
            var rock = Mat("Rock", new Color(.19f, .22f, .25f), .05f, .5f);

            for (int i = 0; i < 45; i++)
            {
                float a = i * 2.39996f;
                float r = 42f + (i % 7) * 5f;
                Vector3 p = new Vector3(Mathf.Cos(a) * r, 1.2f, Mathf.Sin(a) * r);
                Cylinder("TreeTrunk", nature, p, new Vector3(.7f, 2.4f, .7f), trunk);
                Sphere("TreeCanopy", nature, p + Vector3.up * 4.5f, new Vector3(3.8f, 3.4f, 3.8f), leaf);
            }

            for (int i = 0; i < 28; i++)
            {
                float a = i * 2.1f;
                float r = 28f + (i % 5) * 9f;
                Vector3 p = new Vector3(Mathf.Cos(a) * r, .65f, Mathf.Sin(a) * r);
                Sphere("Rock", nature, p, new Vector3(1.8f, 1.1f, 1.5f), rock);
            }
        }

        void BuildWater()
        {
            var water = Mat("Water", new Color(.03f, .28f, .42f), .65f, .9f, .35f);
            Cube("Waterfront", nature, new Vector3(0, .08f, 92f), new Vector3(180f, .12f, 28f), water);
        }

        GameObject Cube(string name, Transform parent, Vector3 pos, Vector3 scale, Material material)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cube);
            go.name = name;
            go.transform.SetParent(parent, false);
            go.transform.localPosition = pos;
            go.transform.localScale = scale;
            go.GetComponent<Renderer>().sharedMaterial = material;
            return go;
        }

        GameObject Cylinder(string name, Transform parent, Vector3 pos, Vector3 scale, Material material)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            go.name = name;
            go.transform.SetParent(parent, false);
            go.transform.localPosition = pos;
            go.transform.localScale = scale;
            go.GetComponent<Renderer>().sharedMaterial = material;
            return go;
        }

        GameObject Sphere(string name, Transform parent, Vector3 pos, Vector3 scale, Material material)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            go.name = name;
            go.transform.SetParent(parent, false);
            go.transform.localPosition = pos;
            go.transform.localScale = scale;
            go.GetComponent<Renderer>().sharedMaterial = material;
            return go;
        }
    }
}
