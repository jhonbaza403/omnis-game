import json
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
            "FactionTerritoryStates",
            "TerrainDeformationSnapshots",
            "Vehicles",
            "WorldElapsedSeconds",
            "SavedAtUtc",
            "Health",
            "MaxHealth",
            "PersistentId",
            "InventoryItems",
            "EquippedItems",
            "AbilityCooldowns",
        )
        for field in required:
            self.assertIn(field, source)

    def test_save_and_load_use_the_same_versioned_slot(self):
        source = (
            ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("SaveCurrentGame", source)
        self.assertIn("LoadCurrentGame", source)
        self.assertIn("CurrentSaveSchemaVersion = 7", source)
        self.assertIn("Save->SchemaVersion != 6", source)
        self.assertIn("Save->SchemaVersion != 2", source)
        self.assertIn("SaveGameToSlot", source)
        self.assertIn("LoadGameFromSlot", source)
        self.assertIn("World->GetNetMode() != NM_Client", source)
        self.assertIn("Inventory->ExportItems()", source)
        self.assertIn("Equipment->ExportEquipment()", source)
        self.assertIn("Abilities->ExportCooldowns()", source)
        self.assertIn("Inventory->ImportItems", source)
        self.assertIn("Equipment->ImportEquipment", source)
        self.assertIn("Abilities->ImportCooldowns", source)
        self.assertIn("ImportTerritoryStates", source)
        self.assertIn("ImportDeformationSnapshots", source)
        self.assertIn("ImportVehicleCondition", source)

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


    def test_inventory_equipment_and_abilities_are_server_authoritative(self):
        inventory_h = (ROOT / "Source/Omnis/Gameplay/Inventory/OmnisInventoryComponent.h").read_text(encoding="utf-8")
        inventory_cpp = (ROOT / "Source/Omnis/Gameplay/Inventory/OmnisInventoryComponent.cpp").read_text(encoding="utf-8")
        equipment_h = (ROOT / "Source/Omnis/Gameplay/Equipment/OmnisEquipmentComponent.h").read_text(encoding="utf-8")
        equipment_cpp = (ROOT / "Source/Omnis/Gameplay/Equipment/OmnisEquipmentComponent.cpp").read_text(encoding="utf-8")
        abilities_h = (ROOT / "Source/Omnis/Gameplay/Abilities/OmnisAbilityComponent.h").read_text(encoding="utf-8")
        abilities_cpp = (ROOT / "Source/Omnis/Gameplay/Abilities/OmnisAbilityComponent.cpp").read_text(encoding="utf-8")
        self.assertIn("ServerRemoveItem", inventory_h)
        self.assertIn("HasMutationAuthority", inventory_cpp)
        self.assertIn("DOREPLIFETIME(UOmnisInventoryComponent, ReplicatedItems)", inventory_cpp)
        self.assertIn("ServerEquip", equipment_h)
        self.assertIn("ServerUnequip", equipment_h)
        self.assertIn("GetQuantity(ItemId) < 1", equipment_cpp)
        self.assertIn("DOREPLIFETIME(UOmnisEquipmentComponent, ReplicatedEquipment)", equipment_cpp)
        self.assertIn("ServerActivateAbility", abilities_h)
        self.assertIn("MulticastAbilityActivated", abilities_h)
        self.assertIn("DOREPLIFETIME(UOmnisAbilityComponent, ReplicatedCooldowns)", abilities_cpp)
        self.assertIn("GetServerWorldTimeSeconds", abilities_cpp)

    def test_legacy_permanent_upgrade_points_are_migrated_to_nexus(self):
        source = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("Save->NexusProgression.PermanentUpgradePoints = FMath::Max(", source)
        self.assertIn("Save->PermanentUpgradePoints", source)
        self.assertIn("Nexus->ImportProgression(Save->NexusProgression)", source)

    def test_save_schema_six_keeps_legacy_zero_two_three_four_and_five_compatible(self):
        save = (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("SchemaVersion = 6", save)
        self.assertIn("Save->SchemaVersion != 0", persistence)
        self.assertIn("Save->SchemaVersion != 2", persistence)
        self.assertIn("Save->SchemaVersion != 3", persistence)
        self.assertIn("Save->SchemaVersion != 4", persistence)
        self.assertIn("Save->SchemaVersion != 5", persistence)
        self.assertIn("CurrentSaveSchemaVersion = 6", persistence)

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


    def test_bootstrap_npcs_have_individual_profiles(self):
        source = (ROOT / "Source/Omnis/World/Environment/OmnisWorldBootstrapSubsystem.cpp").read_text(encoding="utf-8")
        for identity in ("Mara León", "Tomás Herrera", "Occupation.LocalGuide", "Occupation.FieldTechnician", "NPC.Bootstrap.%02d"):
            self.assertIn(identity, source)
        self.assertIn("NPC->CharacterBiography", source)
        self.assertIn("NPC->WorkLocationOffset", source)

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
        automation = (ROOT / "Source/Omnis/Developer/Automation/OmnisPersistenceAutomationTests.cpp").read_text(encoding="utf-8")
        self.assertIn("Mission.Failure.Quest.Escort.Attempt.1", automation)
        self.assertIn("RemoveMemoriesByPrefix", automation)
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



    def test_npc_life_is_batched_with_distance_based_navigation_lod(self):
        manager_h = (ROOT / "Source/Omnis/AI/NPC/OmnisCharacterLifeSubsystem.h").read_text(encoding="utf-8")
        manager_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisCharacterLifeSubsystem.cpp").read_text(encoding="utf-8")
        npc_h = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.h").read_text(encoding="utf-8")
        npc_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn("MaxNPCUpdatesPerTick = 128", manager_h)
        self.assertIn("NavigationSimulationRadius = 6000.0f", manager_h)
        self.assertIn("FOmnisRegisteredNPCEntry", manager_h)
        self.assertIn("FOmnisRegisteredNPCEntry", manager_cpp)
        self.assertNotIn("FRegisteredNPC", manager_cpp)
        self.assertIn("RegisterNPC", manager_cpp)
        self.assertIn("UnregisterNPC", manager_cpp)
        self.assertIn("SimulateLifeFromPopulationManager", manager_cpp)
        self.assertIn("FVector::DistSquared", manager_cpp)
        self.assertIn("Life->RegisterNPC(this)", npc_cpp)
        self.assertIn("Life->UnregisterNPC(this)", npc_cpp)
        self.assertIn("PrimaryActorTick.bCanEverTick = false", npc_cpp)
        self.assertIn("SimulateLifeFromPopulationManager(float DeltaSeconds, bool bAllowNavigation)", npc_h)
        self.assertIn("if (!bAllowNavigation)", npc_cpp)

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



    def test_npc_named_home_and_work_locations_resolve_to_world_positions(self):
        life_h = (ROOT / "Source/Omnis/AI/NPC/OmnisCharacterLifeSubsystem.h").read_text(encoding="utf-8")
        life_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisCharacterLifeSubsystem.cpp").read_text(encoding="utf-8")
        npc = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        world = (ROOT / "Source/Omnis/World/Content/OmnisWorldContentSubsystem.cpp").read_text(encoding="utf-8")
        bootstrap = (ROOT / "Source/Omnis/World/Environment/OmnisWorldBootstrapSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("RegisterLocation(FName LocationId", life_h)
        self.assertIn("ResolveLocation(FName LocationId", life_h)
        self.assertIn("NamedLocations.Add(LocationId, WorldLocation)", life_cpp)
        self.assertIn("Life->ResolveLocation(HomeLocationId", npc)
        self.assertIn("Life->ResolveLocation(WorkLocationId", npc)
        self.assertIn("RegisterLocation(FName(Seed.HomeLocationId)", world)
        self.assertIn("RegisterLocation(NPC->HomeLocationId", bootstrap)



    def test_calendar_and_per_character_weekly_work_schedule(self):
        time_h = (ROOT / "Source/Omnis/Core/Time/OmnisTimeSubsystem.h").read_text(encoding="utf-8")
        time_cpp = (ROOT / "Source/Omnis/Core/Time/OmnisTimeSubsystem.cpp").read_text(encoding="utf-8")
        definition = (ROOT / "Source/Omnis/Data/Characters/OmnisCharacterDefinition.h").read_text(encoding="utf-8")
        npc_h = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.h").read_text(encoding="utf-8")
        npc_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        world = (ROOT / "Source/Omnis/World/Content/OmnisWorldContentSubsystem.cpp").read_text(encoding="utf-8")
        bootstrap = (ROOT / "Source/Omnis/World/Environment/OmnisWorldBootstrapSubsystem.cpp").read_text(encoding="utf-8")
        automation = (ROOT / "Source/Omnis/Developer/Automation/OmnisPersistenceAutomationTests.cpp").read_text(encoding="utf-8")
        self.assertIn("FOmnisWorldCalendarDate GetCalendarDate() const", time_h)
        self.assertIn("BuildCalendarDate(float ElapsedGameSeconds)", time_h)
        self.assertIn("DaysPerSeason = 30", time_cpp)
        self.assertIn("DaysPerYear = DaysPerSeason * 4", time_cpp)
        self.assertIn("Date.DayOfWeek = static_cast<int32>(TotalDays % 7)", time_cpp)
        self.assertIn("int32 WorkDaysMask = 0x7F", definition)
        self.assertIn("int32 WorkDaysMask = 0x7F", npc_h)
        self.assertIn("Time->GetCalendarDate()", npc_cpp)
        self.assertIn("bIsScheduledWorkday &&", npc_cpp)
        self.assertIn("NPC->WorkDaysMask = Seed.WorkDaysMask", world)
        for mask in ("0x1F", "0x3F", "0x7F"):
            self.assertIn(mask, world + bootstrap)
        self.assertIn("Omnis.Time.CalendarBoundaries", automation)



    def test_public_mission_failure_has_unique_rumors_and_auto_witnesses(self):
        definition = (ROOT / "Source/Omnis/Data/Quests/OmnisQuestDefinition.h").read_text(encoding="utf-8")
        quest = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp").read_text(encoding="utf-8")
        state_h = (ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.h").read_text(encoding="utf-8")
        state_cpp = (ROOT / "Source/Omnis/Narrative/Characters/OmnisCharacterStateComponent.cpp").read_text(encoding="utf-8")
        npc = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn("bool bFailureIsPublic = true", definition)
        self.assertIn("float FailureWitnessRadius = 2500.0f", definition)
        self.assertIn("BuildMissionFailureRumorId", quest)
        self.assertIn("Mission.Failure.%s", quest)
        self.assertIn(".Attempt.", quest)
        self.assertIn("WitnessRadiusSquared", quest)
        self.assertIn("!bExplicitWitness && !bNearbyWitness", quest)
        self.assertIn("RemoveMemory(FName EventId)", state_h)
        self.assertIn("RemoveMemoriesByPrefix(const FString& EventPrefix)", state_h)
        self.assertIn("RemoveMemoriesByPrefix", state_cpp)
        self.assertIn("GetQuestAttemptCount(Definition->QuestId)", quest)
        self.assertIn("Memories.RemoveAll", state_cpp)
        self.assertIn("RemoveMemoriesByPrefix(RumorPrefix)", quest)
        self.assertIn("SpreadRumorsTo(SocialTarget)", npc)



    def test_shared_failure_consequence_waits_for_all_linked_recoveries(self):
        quest_h = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.h").read_text(encoding="utf-8")
        quest_cpp = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp").read_text(encoding="utf-8")
        save = (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        replicated = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.cpp").read_text(encoding="utf-8")
        self.assertIn("FailureConsequenceByFailedQuest", quest_h)
        self.assertIn("Recovered", quest_h)
        self.assertIn("Snapshot.State == EOmnisQuestState::Failed", quest_cpp)
        self.assertIn('TEXT("Quest.Recovered")', quest_cpp)
        self.assertIn("FailureConsequenceId", quest_h)
        self.assertIn("BuildQuestScopedFailureConsequenceId", quest_cpp)
        self.assertIn("HasUnresolvedFailureUsingConsequence(BaseConsequenceId)", quest_cpp)
        self.assertIn("RestoreFailureConsequences", persistence)
        self.assertIn("State.FailureConsequenceId = Progress.FailureConsequenceId", replicated)
        self.assertIn("SchemaVersion = 6", save)



    def test_per_quest_failure_consequences_are_scoped_saved_and_replicated(self):
        quest_h = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.h").read_text(encoding="utf-8")
        quest_cpp = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        actor_h = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.h").read_text(encoding="utf-8")
        actor_cpp = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.cpp").read_text(encoding="utf-8")
        automation = (ROOT / "Source/Omnis/Developer/Automation/OmnisPersistenceAutomationTests.cpp").read_text(encoding="utf-8")
        self.assertIn("FailureConsequenceByFailedQuest", quest_h)
        self.assertIn("FName FailureConsequenceId", quest_h)
        self.assertIn("BuildQuestScopedFailureConsequenceId", quest_cpp)
        self.assertIn("HasUnresolvedFailureUsingConsequence(BaseConsequenceId)", quest_cpp)
        self.assertIn("RestoreFailureConsequences()", persistence)
        self.assertIn("FName FailureConsequenceId", actor_h)
        self.assertIn("State.FailureConsequenceId = Progress.FailureConsequenceId", actor_cpp)
        self.assertIn("HasUnresolvedFailureUsingConsequence", automation)
        self.assertIn("Quest.Sabotage", automation)
        self.assertIn("SchemaVersion = 6", (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8"))



    def test_loading_a_save_rebases_population_simulation_clock(self):
        life_h = (ROOT / "Source/Omnis/AI/NPC/OmnisCharacterLifeSubsystem.h").read_text(encoding="utf-8")
        life_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisCharacterLifeSubsystem.cpp").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("RebaseRegisteredNPCsToCurrentTime", life_h)
        self.assertIn("Entry.LastSimulatedGameSeconds = RestoredGameSeconds", life_cpp)
        self.assertIn("Life->RebaseRegisteredNPCsToCurrentTime()", persistence)
        self.assertLess(
            persistence.index("Life->RebaseRegisteredNPCsToCurrentTime()"),
            persistence.index("It->RefreshNarrativeState();", persistence.index("Life->RebaseRegisteredNPCsToCurrentTime()")))


    def test_economy_uses_server_authority_supply_demand_and_atomic_inventory_trades(self):
        header = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("RegisterMarketCommodity", header)
        self.assertIn("GetQuote", header)
        self.assertIn("AdjustMarketConditions", header)
        self.assertIn("ExecuteTrade", header)
        self.assertIn("ExecuteBarter", header)
        self.assertIn("HasMutationAuthority", source)
        self.assertIn("CalculateUnitPrice", source)
        self.assertIn("MinimumPriceIndexBasisPoints", source)
        self.assertIn("MaximumPriceIndexBasisPoints", source)
        self.assertIn("Inventory->AddItem(ItemId, Quantity)", source)
        self.assertIn("Inventory->RemoveItem(ItemId, Quantity)", source)
        self.assertIn("OfferedMarket->CurrencyId != RequestedMarket->CurrencyId", source)
        self.assertIn("CurrentBalance < Total", source)

    def test_economy_state_is_saved_and_only_public_quotes_replicate(self):
        save = (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        economy = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.cpp").read_text(encoding="utf-8")
        actor_h = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.h").read_text(encoding="utf-8")
        actor_cpp = (ROOT / "Source/Omnis/Multiplayer/World/OmnisReplicatedWorldStateActor.cpp").read_text(encoding="utf-8")
        self.assertIn("SchemaVersion = 6", save)
        self.assertIn("FOmnisEconomySnapshot EconomyState", save)
        self.assertIn("StructuralDamageSnapshots", save)
        self.assertIn("Damage->ExportSnapshots()", persistence)
        self.assertIn("Damage->ImportSnapshots(Save->StructuralDamageSnapshots)", persistence)
        self.assertIn("Save->EconomyState = Economy->ExportState()", persistence)
        self.assertIn("Economy->ImportState(Save->EconomyState)", persistence)
        self.assertIn("GetPublicMarketQuotes", economy)
        self.assertIn("if (!Pair.Value.bPubliclyVisible)", economy)
        self.assertIn("GetPublicMarketQuotes() const", actor_h)
        self.assertIn("DOREPLIFETIME(AOmnisReplicatedWorldStateActor, PublicMarketQuotes)", actor_cpp)
        self.assertNotIn("WalletBalances", actor_cpp)

    def test_venezuela_triangle_catalog_has_valid_geography_and_staged_routes(self):
        catalog = json.loads((ROOT / "Content/Omnis/Data/venezuela-triangle-v1.json").read_text(encoding="utf-8"))
        self.assertEqual(catalog["catalog_id"], "omnis.worldpack.venezuela-triangle.v1")
        self.assertEqual(catalog["status"], "design_catalog_not_a_materialized_playable_map")
        self.assertEqual(catalog["world_pack"]["interior_access_target"], 0.80)
        self.assertEqual(catalog["world_pack"]["interior_access_target_status"], "production_target_not_measured")
        self.assertEqual(len(catalog["regions"]), 4)
        self.assertEqual(len(catalog["routes"]), 6)
        self.assertEqual(len(catalog["markets"]), 7)
        route_nodes = set()
        market_ids = {market["id"] for market in catalog["markets"]}
        subregions = {subregion["id"] for region in catalog["regions"] for subregion in region["subregions"]}
        for route in catalog["routes"]:
            route_nodes.update([route["from"], *route["via"], route["to"]])
            for market_id in route["logistics_links"]:
                self.assertIn(market_id, market_ids)
        for node in route_nodes:
            self.assertIn(node, subregions)
        phase_one = set()
        for route in catalog["routes"]:
            if route["phase"] == 1:
                phase_one.update([route["from"], *route["via"], route["to"]])
        for node in ("Caracas.East", "Guarenas", "Guatire", "Caucagua", "RioChico", "Higuerote"):
            self.assertIn(node, phase_one)

    def test_venezuela_economy_seed_matches_worldpack_market_contract(self):
        catalog = json.loads((ROOT / "Content/Omnis/Data/venezuela-triangle-v1.json").read_text(encoding="utf-8"))
        economy = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.cpp").read_text(encoding="utf-8")
        world_content = (ROOT / "Source/Omnis/World/Content/OmnisWorldContentSubsystem.cpp").read_text(encoding="utf-8")
        for market in catalog["markets"]:
            self.assertIn('TEXT("' + market["id"] + '")', economy)
            for item_id in market["commodities"]:
                self.assertIn('TEXT("' + item_id + '")', economy)
        self.assertIn("RegisterVenezuelaTriangleMarkets", world_content)
        self.assertIn("VenezuelaTriangle", world_content)
        self.assertIn("Seed.Phase > SafeThroughPhase", economy)
        self.assertIn("SafeThroughPhase = FMath::Clamp(ThroughPhase, 1, 3)", economy)
        self.assertIn('TEXT("Region.Barlovento")', economy)



    def test_structural_damage_sections_persist_across_world_cell_unload(self):
        component_h = (ROOT / "Source/Omnis/World/Destruction/OmnisStructuralDamageComponent.h").read_text(encoding="utf-8")
        component_cpp = (ROOT / "Source/Omnis/World/Destruction/OmnisStructuralDamageComponent.cpp").read_text(encoding="utf-8")
        subsystem_h = (ROOT / "Source/Omnis/World/Destruction/OmnisStructuralDamageSubsystem.h").read_text(encoding="utf-8")
        subsystem_cpp = (ROOT / "Source/Omnis/World/Destruction/OmnisStructuralDamageSubsystem.cpp").read_text(encoding="utf-8")
        save = (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("PersistentStructureId", component_h)
        self.assertIn("ApplyStructuralDamage", component_h)
        self.assertIn("RepairStructuralPart", component_h)
        self.assertIn("ReplicatedUsing=OnRep_PartStates", component_h)
        self.assertIn("RecordSnapshot(ExportSnapshot())", component_cpp)
        self.assertIn("StructureSnapshots", subsystem_h)
        self.assertIn("StructureSnapshots.Add", subsystem_cpp)
        self.assertIn("RegisterStructure(Component)", subsystem_cpp)
        self.assertIn("StructuralDamageSnapshots", save)
        self.assertIn("Damage->ExportSnapshots()", persistence)
        self.assertIn("Damage->ImportSnapshots(Save->StructuralDamageSnapshots)", persistence)

    def test_community_mod_manifest_is_data_only_signed_and_reference_valid(self):
        manifest = json.loads((ROOT / "Content/Omnis/Mods/Samples/manifest.json").read_text(encoding="utf-8"))
        events = json.loads((ROOT / "Content/Omnis/Mods/Samples/data/world-events.json").read_text(encoding="utf-8"))
        catalog = json.loads((ROOT / "Content/Omnis/Data/venezuela-triangle-v1.json").read_text(encoding="utf-8"))
        validator = (ROOT / "CI/Mods/validate_mod_manifest.mjs").read_text(encoding="utf-8")
        self.assertEqual(manifest["package_status"], "unsigned_authoring_template_not_installable")
        self.assertEqual(manifest["signature_policy"]["algorithm"], "Ed25519")
        self.assertTrue(manifest["signature_policy"]["required_for_production"])
        for permission in ("execute_scripts", "execute_native_code", "filesystem_access", "external_network_access"):
            self.assertFalse(manifest["permissions"][permission])
        route_ids = {route["id"] for route in catalog["routes"]}
        market_items = {market["id"]: set(market["commodities"]) for market in catalog["markets"]}
        self.assertEqual(events["package_id"], manifest["package_id"])
        for event in events["events"]:
            for route_id in event["route_ids"]:
                self.assertIn(route_id, route_ids)
            for effect in event["effects"]:
                self.assertIn(effect["market_id"], market_items)
                self.assertIn(effect["item_id"], market_items[effect["market_id"]])
        self.assertIn("execute_native_code", validator)
        self.assertIn("Path traversal is forbidden", validator)


    def test_route_disruptions_change_only_linked_regional_market_supply(self):
        economy_h = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.h").read_text(encoding="utf-8")
        economy_cpp = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.cpp").read_text(encoding="utf-8")
        quest_h = (ROOT / "Source/Omnis/Data/Quests/OmnisQuestDefinition.h").read_text(encoding="utf-8")
        quest_cpp = (ROOT / "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp").read_text(encoding="utf-8")
        automation = (ROOT / "Source/Omnis/Developer/Automation/OmnisPersistenceAutomationTests.cpp").read_text(encoding="utf-8")
        self.assertIn("ApplyRouteSupplyShock", economy_h)
        self.assertIn("Route.Caucagua-RioChico-Higuerote", economy_cpp)
        self.assertIn("Market.Caucagua.Rural", economy_cpp)
        self.assertIn("Market.RioChico.Coastal", economy_cpp)
        self.assertIn("Market.Higuerote.Coastal", economy_cpp)
        self.assertIn("FailureAffectedRouteIds", quest_h)
        self.assertIn("FailureMarketSupplyDelta", quest_h)
        self.assertIn("RecoveryAffectedRouteIds", quest_h)
        self.assertIn("RecoveryMarketSupplyDelta", quest_h)
        self.assertIn("Economy->ApplyRouteSupplyShock", quest_cpp)
        self.assertIn("A disrupted coastal route raises local fuel price", automation)


    def test_building_interior_registry_measures_the_80_percent_target_without_claiming_completion(self):
        interiors = json.loads((ROOT / "Content/Omnis/Data/building-interiors-v1.json").read_text(encoding="utf-8"))
        world = json.loads((ROOT / "Content/Omnis/Data/venezuela-triangle-v1.json").read_text(encoding="utf-8"))
        self.assertEqual(interiors["target_metric"]["target_ratio"], 0.80)
        self.assertEqual(interiors["target_metric"]["planned_building_inventory_count"], 20)
        self.assertEqual(interiors["target_metric"]["planned_accessible_building_target"], 16)
        self.assertEqual(interiors["target_metric"]["measured_accessible_buildings"], 0)
        self.assertIsNone(interiors["target_metric"]["measured_ratio"])
        self.assertEqual(interiors["target_metric"]["measurement_status"], "not_measured_no_native_maps_or_building_assets_verified")
        subregions = {subregion["id"] for region in world["regions"] for subregion in region["subregions"]}
        self.assertEqual(len(interiors["buildings"]), 20)
        self.assertEqual(len({building["persistent_id"] for building in interiors["buildings"]}), 20)
        for building in interiors["buildings"]:
            self.assertIn(building["subregion_id"], subregions)
            self.assertEqual(building["materialization_status"], "not_materialized")
            self.assertEqual(building["accessibility_test_status"], "not_tested")
            self.assertTrue(building["room_templates"])
            self.assertTrue(building["vertical_routes"])



if __name__ == "__main__":
    unittest.main()
