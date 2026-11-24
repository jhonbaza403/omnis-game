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

    def test_character_profile_drives_routine_and_identity(self):
        definition = (ROOT / "Source/Omnis/Data/Characters/OmnisCharacterDefinition.h").read_text(encoding="utf-8")
        npc_h = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.h").read_text(encoding="utf-8")
        npc_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        for field in ("Biography", "OccupationId", "HomeLocationId", "WorkLocationId", "WorkStartHour", "RestStartHour", "HungerThreshold", "ExploreRadius", "VisualMesh"):
            self.assertIn(field, definition)
        self.assertIn("CharacterDefinition", npc_h)
        self.assertIn("ApplyCharacterDefinition", npc_cpp)
        self.assertIn("IsWithinHourWindow", npc_cpp)
        self.assertIn("CharacterDisplayName", npc_cpp)

    def test_production_world_spawns_stable_citizens(self):
        source = (ROOT / "Source/Omnis/World/Content/OmnisWorldContentSubsystem.cpp").read_text(encoding="utf-8")
        for citizen in ("Citizen.OpenWorld.Civilian.001", "Citizen.OpenWorld.Shopkeeper.001", "Citizen.OpenWorld.Police.001", "Citizen.OpenWorld.Medic.001", "Citizen.OpenWorld.Pilot.001"):
            self.assertIn(citizen, source)
        self.assertIn("SpawnActorDeferred<AOmnisNPCCharacter>", source)
        self.assertIn("NPC->PersistentId = StableId", source)


    def test_production_citizens_have_distinct_names_and_biographies(self):
        source = (ROOT / "Source/Omnis/World/Content/OmnisWorldContentSubsystem.cpp").read_text(encoding="utf-8")
        for identity in ("Elena Ríos", "Mateo Cruz", "Lucía Navarro", "Daniel Vega", "Sara Molina", "Andrés Salas"):
            self.assertIn(identity, source)
        self.assertIn("OccupationId", source)
        self.assertIn("HomeLocationId", source)
        self.assertIn("WorkLocationId", source)
        self.assertIn("CharacterBiography", source)

    def test_npc_identity_and_persona_are_replicated(self):
        header = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        for field in ("CharacterId", "PersistentId", "CharacterDisplayName", "CharacterBiography", "OccupationId", "HomeLocationId", "WorkLocationId"):
            self.assertIn(f"DOREPLIFETIME(AOmnisNPCCharacter, {field})", source)
        for field in ("CharacterId", "PersistentId", "CharacterDisplayName", "CharacterBiography", "OccupationId", "HomeLocationId", "WorkLocationId"):
            self.assertIn("Replicated", header)
            self.assertIn(field, header)


    def test_failure_rumors_spread_with_bounded_decay_and_deduplication(self):
        npc = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        state = (ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.cpp").read_text(encoding="utf-8")
        quest = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("SpreadRumorsTo(SocialTarget)", npc)
        self.assertIn("RumorSpreadLoss = 20", npc)
        self.assertIn("MinimumRumorStrength = 30", npc)
        self.assertIn("TargetState->HasMemory(Memory.EventId)", npc)
        self.assertIn("HasMemoryPrefix", state)
        self.assertIn("RumorSpreadLoss", npc)
        automation = (ROOT / "Source/Omnis/Developer/Automation/OmnisPersistenceAutomationTests.cpp").read_text(encoding="utf-8")
        self.assertIn("HasMemoryPrefix", automation)
        self.assertIn("Definition->FailureReputationTargetId", quest)

    def test_state_and_economic_mutations_are_server_authoritative(self):
        state = (ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.cpp").read_text(encoding="utf-8")
        reputation = (ROOT / "Source/Omnis/Narrative/Reputation/OmnisReputationSubsystem.cpp").read_text(encoding="utf-8")
        factions = (ROOT / "Source/Omnis/Narrative/Factions/OmnisFactionSubsystem.cpp").read_text(encoding="utf-8")
        consequences = (ROOT / "Source/Omnis/Narrative/Consequences/OmnisConsequenceSubsystem.cpp").read_text(encoding="utf-8")
        world = (ROOT / "Source/Omnis/World/WorldState/OmnisWorldStateSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("HasMutationAuthority", state)
        npc = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn("NPC.SocialEncounter", npc)
        self.assertIn("Player.SocialEncounter", npc)
        self.assertIn("!Cast<AOmnisPlayerCharacter>(SocialTarget)", npc)
        for source in (reputation, factions, consequences, world):
            self.assertIn("GetNetMode() == NM_Client", source)

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


    def test_health_component_notifies_clients_on_replication(self):
        header = (ROOT / "Source/Omnis/Systems/Damage/OmnisHealthComponent.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/Omnis/Systems/Damage/OmnisHealthComponent.cpp").read_text(encoding="utf-8")
        self.assertIn("ReplicatedUsing=OnRep_Health", header)
        self.assertIn("ReplicatedUsing=OnRep_Dead", header)
        self.assertIn("OnHealthChanged.Broadcast(Health, Health - PreviousHealth)", source)
        self.assertIn("OnDeath.Broadcast()", source)

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
