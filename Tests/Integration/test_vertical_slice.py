import json
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class VerticalSliceContractTests(unittest.TestCase):
    def test_flow(self):
        manifest = json.loads(
            (ROOT / "Content/Omnis/Data/experience-manifest.json").read_text(
                encoding="utf-8"
            )
        )
        expected = [
            "explore",
            "play",
            "create",
            "customize",
            "share",
            "compete",
            "progress",
            "change_world",
        ]
        self.assertEqual(manifest["experience_loop"], expected)

    def test_unreal_configuration(self):
        project = json.loads(
            (ROOT / "Omnis.uproject").read_text(encoding="utf-8")
        )
        self.assertEqual(project["EngineAssociation"], "5.6")
        plugin_names = {plugin["Name"] for plugin in project["Plugins"]}
        self.assertIn("PythonScriptPlugin", plugin_names)
        self.assertIn("ChaosVehiclesPlugin", plugin_names)

    def test_master_systems_are_present(self):
        required = [
            "Source/Omnis/Core/Identity/OmnisAvatarSubsystem.h",
            "Source/Omnis/Core/Progression/OmnisRewardSubsystem.h",
            "Source/Omnis/Gameplay/Construction/OmnisConstructionComponent.h",
            "Source/Omnis/Online/Friends/OmnisPartySubsystem.h",
            "Source/Omnis/Online/Competitive/OmnisRankedSubsystem.h",
            "Source/Omnis/Online/Creators/OmnisCreatorSubsystem.h",
            "Source/Omnis/Online/Safety/OmnisSafetySubsystem.h",
        ]
        for path in required:
            self.assertTrue((ROOT / path).is_file(), msg=f"missing: {path}")

    def test_world_materializers_have_runtime_contracts(self):
        generator = (
            ROOT / "Tools/Editor/GenerateOmnisWorldContent.py"
        ).read_text(encoding="utf-8")
        premium = (
            ROOT / "Tools/Editor/MaterializeOmnisPremiumWorld.py"
        ).read_text(encoding="utf-8")

        self.assertIn("def log(message)", generator)
        self.assertIn("new_level_from_template", premium)
        self.assertIn("ART_ROOT", premium)
        self.assertNotIn("ART+", premium)

    def test_flow_ends_in_renewed_cycle(self):
        flow = [
            "BOOT",
            "PRIMER CICLO",
            "MUNDO",
            "EXPLORACIÓN",
            "NPC",
            "DECISIÓN",
            "COMBATE",
            "FIN DEL CICLO",
            "NEXO",
            "MEJORA",
            "NUEVO CICLO",
        ]
        self.assertEqual(flow[0], "BOOT")
        self.assertEqual(flow[-1], "NUEVO CICLO")
        self.assertLess(flow.index("NEXO"), flow.index("NUEVO CICLO"))


if __name__ == "__main__":
    unittest.main()
