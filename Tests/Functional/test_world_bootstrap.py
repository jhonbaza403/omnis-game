import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

class WorldBootstrapTests(unittest.TestCase):
    def test_world_manifest(self):
        data = json.loads((ROOT / "Content/Omnis/World/world-manifest.json").read_text())
        self.assertGreater(data["cell_size"], 0)
        self.assertTrue(data["regions"])

if __name__ == "__main__":
    unittest.main()
