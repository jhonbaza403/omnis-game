from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]
SAVE_HEADER = (ROOT / "Source/Omnis/Core/Save/OmnisSaveSubsystem.h").read_text(encoding="utf-8")
SAVE_CPP = (ROOT / "Source/Omnis/Core/Save/OmnisSaveSubsystem.cpp").read_text(encoding="utf-8")
CLOUD_HEADER = (ROOT / "Source/Omnis/Online/Persistence/OmnisCloudPersistenceSubsystem.h").read_text(encoding="utf-8")

class PersistenceContractTests(unittest.TestCase):
    def test_local_save_has_ten_slots(self):
        self.assertIn("MaxSlot = 10", SAVE_HEADER)
        self.assertIn("SaveGameToSlot", SAVE_CPP)
        self.assertIn("LoadGameFromSlot", SAVE_CPP)

    def test_cloud_persistence_covers_save_load_delete_and_events(self):
        for symbol in ("SaveSlotJson", "LoadSlotJson", "DeleteSlot", "SubmitEvent"):
            self.assertIn(symbol, CLOUD_HEADER)

if __name__ == "__main__":
    unittest.main()
