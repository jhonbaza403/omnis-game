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
        self.assertIn("CurrentSaveSchemaVersion = 3", source)
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

    def test_save_schema_three_keeps_legacy_zero_and_two_compatible(self):
        save = (ROOT / "Source/Omnis/Core/Save/OmnisSaveGame.h").read_text(encoding="utf-8")
        persistence = (ROOT / "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("SchemaVersion = 3", save)
        self.assertIn("Save->SchemaVersion != 0", persistence)
        self.assertIn("Save->SchemaVersion != 2", persistence)
        self.assertIn("CurrentSaveSchemaVersion = 3", persistence)

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


if __name__ == "__main__":
    unittest.main()
