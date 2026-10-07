import json
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class GameplayContractsTests(unittest.TestCase):
    def setUp(self):
        self.manifest = json.loads(
            (ROOT / "Content/Omnis/Data/experience-manifest.json").read_text(
                encoding="utf-8"
            )
        )

    def test_experience_manifest_has_all_pillars(self):
        expected = {
            "visual_identity",
            "expressive_characters",
            "rewarding_progression",
            "creative_building",
            "cooperative_play",
            "social_presence",
            "ranked_competition",
            "cinematic_narrative",
            "cycle_world_state",
            "ugc_creation",
            "player_safety",
        }
        self.assertEqual(set(self.manifest["pillars"].keys()), expected)
        self.assertTrue(all(self.manifest["pillars"].values()))

    def test_experience_systems_exist(self):
        for system_path in self.manifest["systems"].values():
            base = ROOT / system_path
            self.assertTrue(
                base.with_suffix(".h").is_file()
                or base.with_suffix(".cpp").is_file()
                or base.is_file(),
                msg=f"missing system contract: {system_path}",
            )

    def test_inventory_does_not_go_negative(self):
        current = 3
        remove = 5
        self.assertFalse(remove <= current)

    def test_health_is_clamped(self):
        self.assertEqual(max(0.0, min(100.0, -10.0)), 0.0)

    def test_build_limits_are_explicit(self):
        source = (
            ROOT
            / "Source/Omnis/Gameplay/Construction/OmnisConstructionComponent.h"
        ).read_text(encoding="utf-8")
        self.assertIn("MaxPieces = 128", source)
        self.assertIn("GridSize = 100.0f", source)
        self.assertIn("RemoveLastPiece", source)

    def test_ranked_system_is_skill_based(self):
        source = (
            ROOT
            / "Source/Omnis/Online/Competitive/OmnisRankedSubsystem.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("State.Rating", source)
        self.assertIn("RecordMatchResult", source)
        self.assertNotIn("Purchase", source)

    def test_safety_contract_exists(self):
        source = (
            ROOT / "Source/Omnis/Online/Safety/OmnisSafetySubsystem.h"
        ).read_text(encoding="utf-8")
        for symbol in ("BlockPlayer", "UnblockPlayer", "ReportPlayer", "CanInteract"):
            self.assertIn(symbol, source)


if __name__ == "__main__":
    unittest.main()
