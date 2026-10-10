using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;

namespace Omnis.Editor
{
    /// <summary>
    /// Reports whether real, imported production art exists. It never treats LFS pointer text
    /// or procedural fallback geometry as a completed production asset.
    /// </summary>
    public static class OmnisArtAssetAudit
    {
        private const string Root = "Assets/Omnis/Art/Imported";
        private const string ReportPath = "TestResults/omnis-art-readiness.json";

        [Serializable]
        private sealed class Report
        {
            public string generatedUtc;
            public int importedMeshFiles;
            public int characterPrefabs;
            public int environmentPrefabs;
            public int vegetationPrefabs;
            public int pbrMaterials;
            public int materialsWithNormalMaps;
            public int lfsPointerFiles;
            public string status;
            public string[] blockers;
        }

        [MenuItem("Omnis/Art/Audit Real Production Assets")]
        public static void RunAudit()
        {
            Directory.CreateDirectory(Path.Combine(Directory.GetParent(Application.dataPath).FullName, "TestResults"));
            string absoluteRoot = Path.Combine(Application.dataPath, "Omnis/Art/Imported");
            Directory.CreateDirectory(Path.Combine(absoluteRoot, "Characters"));
            Directory.CreateDirectory(Path.Combine(absoluteRoot, "Environment"));
            Directory.CreateDirectory(Path.Combine(absoluteRoot, "Nature"));
            Directory.CreateDirectory(Path.Combine(absoluteRoot, "Materials"));
            AssetDatabase.Refresh();

            Report report = BuildReport(absoluteRoot);
            string output = Path.Combine(Directory.GetParent(Application.dataPath).FullName, ReportPath);
            File.WriteAllText(output, JsonUtility.ToJson(report, true) + Environment.NewLine);

            if (report.blockers.Length == 0)
                Debug.Log("[Omnis Art Audit] PASS: imported production art thresholds met. Report: " + output);
            else
                Debug.LogWarning("[Omnis Art Audit] NOT READY: " + string.Join("; ", report.blockers) + ". Report: " + output);
        }

        private static Report BuildReport(string absoluteRoot)
        {
            var blockers = new List<string>();
            int meshes = 0;
            int lfsPointers = 0;

            if (Directory.Exists(absoluteRoot))
            {
                foreach (string file in Directory.GetFiles(absoluteRoot, "*", SearchOption.AllDirectories))
                {
                    string extension = Path.GetExtension(file).ToLowerInvariant();
                    if (extension != ".fbx" && extension != ".obj" && extension != ".glb" && extension != ".gltf") continue;

                    if (IsLfsPointer(file))
                    {
                        lfsPointers++;
                        continue;
                    }
                    meshes++;
                }
            }

            string[] prefabs = AssetDatabase.FindAssets("t:Prefab", new[] { Root });
            int characters = 0;
            int environment = 0;
            int vegetation = 0;
            foreach (string guid in prefabs)
            {
                string path = AssetDatabase.GUIDToAssetPath(guid);
                GameObject prefab = AssetDatabase.LoadAssetAtPath<GameObject>(path);
                if (prefab == null) continue;
                string lower = path.ToLowerInvariant();
                bool hasSkinnedMesh = prefab.GetComponentInChildren<SkinnedMeshRenderer>(true) != null;
                if (hasSkinnedMesh && (lower.Contains("character") || lower.Contains("hero") || lower.Contains("player")))
                    characters++;
                if (lower.Contains("environment") || lower.Contains("architecture") || lower.Contains("ruin") || lower.Contains("rock"))
                    environment++;
                if (lower.Contains("nature") || lower.Contains("tree") || lower.Contains("plant") || lower.Contains("vegetation"))
                    vegetation++;
            }

            string[] materialGuids = AssetDatabase.FindAssets("t:Material", new[] { Root });
            int pbr = 0;
            int normals = 0;
            foreach (string guid in materialGuids)
            {
                Material material = AssetDatabase.LoadAssetAtPath<Material>(AssetDatabase.GUIDToAssetPath(guid));
                if (material == null) continue;
                bool hasBase = (material.HasProperty("_BaseMap") && material.GetTexture("_BaseMap") != null) ||
                               (material.HasProperty("_MainTex") && material.GetTexture("_MainTex") != null);
                if (hasBase) pbr++;
                if (material.HasProperty("_BumpMap") && material.GetTexture("_BumpMap") != null) normals++;
            }

            if (meshes < 1) blockers.Add("no imported FBX/OBJ/GLB/GLTF mesh files");
            if (characters < 1) blockers.Add("no production character prefab with a SkinnedMeshRenderer");
            if (environment < 3) blockers.Add("fewer than three imported environment/architecture prefabs");
            if (vegetation < 3) blockers.Add("fewer than three imported vegetation prefabs");
            if (pbr < 6) blockers.Add("fewer than six textured PBR materials");
            if (normals < 3) blockers.Add("fewer than three materials with normal maps");
            if (lfsPointers > 0) blockers.Add(lfsPointers + " mesh files are still Git LFS pointer stubs");

            return new Report
            {
                generatedUtc = DateTime.UtcNow.ToString("O"),
                importedMeshFiles = meshes,
                characterPrefabs = characters,
                environmentPrefabs = environment,
                vegetationPrefabs = vegetation,
                pbrMaterials = pbr,
                materialsWithNormalMaps = normals,
                lfsPointerFiles = lfsPointers,
                status = blockers.Count == 0 ? "ready-for-visual-review" : "procedural-prototype-only",
                blockers = blockers.ToArray()
            };
        }

        private static bool IsLfsPointer(string path)
        {
            using (var reader = new StreamReader(path))
            {
                string firstLine = reader.ReadLine();
                return firstLine != null && firstLine.StartsWith("version https://git-lfs.github.com/spec/v1", StringComparison.Ordinal);
            }
        }
    }
}
