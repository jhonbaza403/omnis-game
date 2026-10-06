import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

class DataDrivenContentTests(unittest.TestCase):
    def test_balance_targets(self):
        data = json.loads((ROOT / "Content/Omnis/Data/balance.json").read_text(encoding="utf-8"))
        self.assertEqual(data["performanceTargets"]["targetHz"], 60)
        self.assertLessEqual(data["performanceTargets"]["frameBudgetMs"], 16.67)
        self.assertLess(data["performanceTargets"]["memoryWarningRatio"], data["performanceTargets"]["memoryCriticalRatio"])

    def test_narrative_is_authored(self):
        data = json.loads((ROOT / "Content/Omnis/Data/narrative.json").read_text(encoding="utf-8"))
        self.assertGreaterEqual(len(data["beats"]), 5)
        self.assertTrue(all(beat["text"].strip() for beat in data["beats"]))

    def test_encounter_ids_are_unique(self):
        data = json.loads((ROOT / "Content/Omnis/Data/encounters.json").read_text(encoding="utf-8"))
        ids = [entry["id"] for entry in data["encounters"]]
        self.assertEqual(len(ids), len(set(ids)))

if __name__ == "__main__":
    unittest.main()
