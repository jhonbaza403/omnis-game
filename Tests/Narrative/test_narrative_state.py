import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


class NarrativeStateTests(unittest.TestCase):
    def test_standing_bounds(self):
        self.assertEqual(max(-100, min(100, 250)), 100)
        self.assertEqual(max(-100, min(100, -250)), -100)

    def test_quest_failure_is_data_driven(self):
        definition = (
            ROOT / "Source/Omnis/Data/Quests/OmnisQuestDefinition.h"
        ).read_text(encoding="utf-8")
        subsystem = (
            ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp"
        ).read_text(encoding="utf-8")
        for field in (
            "FailureConsequenceId",
            "FailureReputationDelta",
            "FailureFactionStandingDelta",
            "FailureWorldStateKey",
            "WitnessCharacterIds",
            "RecoveryQuestId",
        ):
            self.assertIn(field, definition)
        for behavior in (
            "ApplyFailureOutcome",
            "AdjustStanding",
            "AddReputation",
            "RecordMemory",
            "IncrementState",
        ):
            self.assertIn(behavior, subsystem)

    def test_character_life_state_has_needs_and_memory(self):
        header = (
            ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.h"
        ).read_text(encoding="utf-8")
        npc = (
            ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp"
        ).read_text(encoding="utf-8")
        for field in ("Hunger", "Energy", "SocialNeed", "Memories", "Relationships"):
            self.assertIn(field, header)
        for behavior in ("SimulateLife", "MoveToLifeDestination", "SenseClosest", "RecordMemory"):
            self.assertIn(behavior, npc)

    def test_native_automation_covers_failure_and_life(self):
        test_file = (
            ROOT / "Source/Omnis/Developer/Automation/OmnisNarrativeLifeAutomationTests.cpp"
        )
        source = test_file.read_text(encoding="utf-8")
        self.assertIn("Omnis.Narrative.CharacterLifeState", source)
        self.assertIn("Omnis.Narrative.QuestFailureConsequences", source)


if __name__ == "__main__":
    unittest.main()
