import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

class LayoutTests(unittest.TestCase):
    def test_no_gitkeep_placeholders(self):
        self.assertEqual(list(ROOT.rglob(".gitkeep")), [])

    def test_core_files_exist(self):
        required = [
            "Source/Omnis/Core/Cycle/OmnisCycleSubsystem.cpp",
            "Source/Omnis/Core/Nexus/OmnisNexusSubsystem.cpp",
            "Source/Omnis/Core/Progression/OmnisProgressionSubsystem.cpp",
        ]
        for path in required:
            self.assertTrue((ROOT / path).is_file(), path)

if __name__ == "__main__":
    unittest.main()
