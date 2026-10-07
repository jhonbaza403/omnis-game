import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


class VisualVerticalSliceTests(unittest.TestCase):
    def test_visual_runtime_components_exist(self):
        required = [
            ROOT / "Source/Omnis/World/Environment/OmnisWorldBootstrapSubsystem.cpp",
            ROOT / "Source/Omnis/World/Environment/OmnisNexusVisualActor.cpp",
            ROOT / "Source/Omnis/World/Environment/OmnisNexusVisualActor.h",
            ROOT / "Source/Omnis/UI/HUD/OmnisHUD.cpp",
            ROOT / "Tools/Editor/GenerateOmnisVerticalSlice.py",
        ]
        for path in required:
            self.assertTrue(path.is_file(), str(path))

    def test_visual_manifest_is_machine_readable(self):
        manifest = json.loads(
            (ROOT / "Content/Omnis/World/world-manifest.json").read_text(
                encoding="utf-8"
            )
        )
        self.assertEqual(manifest["version"], 1)
        self.assertIn("PrimerCiclo", [region["id"] for region in manifest["regions"]])


if __name__ == "__main__":
    unittest.main()
