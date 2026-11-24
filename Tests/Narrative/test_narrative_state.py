import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


class PersistenceContractTests(unittest.TestCase):
    def test_save_schema_contains_world_narrative_and_character_state(self):
        source = (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8")
        required = (
            "SchemaVersion",
            "CycleState",
            "NexusProgression",
            "QuestProgress",
            "ActiveConsequences",
            "Characters",
            "WorldStateValues",
            "ReputationValues",
            "FactionStandings",
            "WorldElapsedSeconds",
            "SavedAtUtc",
            "Health",
            "MaxHealth",
            "PersistentId",
        )
        for field in required:
            self.assertIn(field, source)

    def test_save_and_load_use_the_same_versioned_slot(self):
        source = (
            ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("SaveCurrentGame", source)
        self.assertIn("LoadCurrentGame", source)
        self.assertIn("CurrentSaveSchemaVersion = 2", source)
        self.assertIn("SaveGameToSlot", source)
        self.assertIn("LoadGameFromSlot", source)
        self.assertIn("World->GetNetMode() != NM_Client", source)

    def test_npc_identity_is_not_the_archetype_name(self):
        npc = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        persistence = (
            ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("GetPersistentId() const", npc)
        self.assertIn("return PersistentId;", npc)
        self.assertIn("NPC->GetPersistentId()", persistence)
        self.assertIn("RestorePersistentState", persistence)
        self.assertIn("Character.PersistentId = PersistentId", persistence)

    def test_recovery_quest_can_remove_configured_consequences(self):
        definition = (
            ROOT / "Source/Omnis/Data/Quests/OmnisQuestDefinition.h"
        ).read_text(encoding="utf-8")
        subsystem = (
            ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("ConsequencesToRemoveOnCompletion", definition)
        self.assertIn("RecoveryReputationDelta", definition)
        self.assertIn("RecoveryRelationshipDelta", definition)
        self.assertIn("ApplyCompletionOutcome", subsystem)
        self.assertIn("RemoveConsequence", subsystem)
        self.assertIn("RecoveryWitnessCharacterIds", subsystem)

    def test_character_snapshot_restores_bounded_relationships_and_memory(self):
        header = (
            ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.h"
        ).read_text(encoding="utf-8")
        source = (
            ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("FOmnisCharacterLifeSnapshot", header)
        self.assertIn("ExportSnapshot", source)
        self.assertIn("ImportSnapshot", source)
        self.assertIn("FMath::Clamp(Pair.Value, -100, 100)", source)
        reputation = (ROOT / "Source/Omnis/Narrative/Reputation/OmnisReputationSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("FMath::Clamp", reputation)
        self.assertIn("-100", reputation)
        self.assertIn("100", reputation)

    def test_authoritative_gameplay_uses_server_rpcs(self):
        interaction = (ROOT / "Source/Omnis/Gameplay/Interaction/OmnisInteractionComponent.cpp").read_text(encoding="utf-8")
        combat = (ROOT / "Source/Omnis/Gameplay/Combat/OmnisCombatComponent.cpp").read_text(encoding="utf-8")
        construction = (ROOT / "Source/Omnis/Gameplay/Construction/OmnisConstructionComponent.cpp").read_text(encoding="utf-8")
        controller = (ROOT / "Source/Omnis/Gameplay/Player/OmnisPlayerController.cpp").read_text(encoding="utf-8")
        self.assertIn("ServerInteract();", interaction)
        self.assertIn("PerformInteractionOnAuthority", interaction)
        self.assertIn("ServerAttack();", combat)
        self.assertIn("PerformAttackOnAuthority", combat)
        self.assertIn("ServerRemoveLastPiece();", construction)
        self.assertIn("DOREPLIFETIME(UOmnisConstructionComponent, BuiltPieces)", construction)
        self.assertIn("ServerToggleNearestVehicle();", controller)
        self.assertIn("ServerToggleNearestAircraft();", controller)

    def test_mission_state_is_replicated_through_existing_world_actor(self):
        actor_h = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.h").read_text(encoding="utf-8")
        actor_cpp = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.cpp").read_text(encoding="utf-8")
        quest_cpp = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp").read_text(encoding="utf-8")
        for field in ("QuestStates", "ActiveConsequences", "ReputationValues", "FactionStandings", "WorldValues"):
            self.assertIn(field, actor_h)
            self.assertIn(f"DOREPLIFETIME(AOmnisReplicatedWorldStateActor, {field})", actor_cpp)
        self.assertIn("RefreshNarrativeState", actor_cpp)
        self.assertIn("State.RecoveryQuestId = Progress.RecoveryQuestId", actor_cpp)
        self.assertIn("RefreshNarrativeReplication", quest_cpp)

    def test_npc_and_life_state_replicate_from_authority(self):
        npc = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        header = (ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.cpp").read_text(encoding="utf-8")
        self.assertIn("SetReplicates(true)", npc)
        self.assertIn("SetReplicateMovement(true)", npc)
        self.assertIn("if (!HasAuthority())", npc)
        self.assertIn("ReplicatedRelationships", header)
        self.assertIn("DOREPLIFETIME(UOmnisCharacterStateComponent, ReplicatedRelationships)", source)
        self.assertIn("SetIsReplicatedByDefault(true)", source)


if __name__ == "__main__":
    unittest.main()
