#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Core/Save/OmnisSaveGame.h"
#include "Core/Time/OmnisTimeSubsystem.h"
#include "Gameplay/Abilities/OmnisAbilityComponent.h"
#include "Gameplay/Equipment/OmnisEquipmentComponent.h"
#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"
#include "Narrative/Quests/OmnisQuestSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisPersistenceSnapshotRoundTrip,
    "Omnis.Persistence.SnapshotRoundTrip",
    EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter)

bool FOmnisPersistenceSnapshotRoundTrip::RunTest(const FString& Parameters)
{
    UOmnisCharacterStateComponent* OriginalState =
        NewObject<UOmnisCharacterStateComponent>();
    UOmnisCharacterStateComponent* RestoredState =
        NewObject<UOmnisCharacterStateComponent>();
    TestNotNull(TEXT("Original character state"), OriginalState);
    TestNotNull(TEXT("Restored character state"), RestoredState);
    if (!OriginalState || !RestoredState)
    {
        return false;
    }

    OriginalState->SetRelationship(TEXT("Player"), -65);
    OriginalState->RecordMemory(TEXT("Quest.Failed"), TEXT("Quest.Escort"), 90);
    OriginalState->RecordMemory(TEXT("Mission.Failure.PublicShame"), TEXT("Player"), 100);
    OriginalState->RecordMemory(TEXT("Mission.Failure.Quest.Escort.Attempt.1"), TEXT("Player"), 100);
    OriginalState->SimulateNeeds(10.0f, EOmnisCharacterLifeGoal::Work, false);
    const FOmnisCharacterLifeSnapshot LifeSnapshot = OriginalState->ExportSnapshot();
    RestoredState->ImportSnapshot(LifeSnapshot);

    TestEqual(TEXT("Relationship survives snapshot"), RestoredState->GetRelationship(TEXT("Player")), -65);
    TestEqual(TEXT("Memory count survives snapshot"), RestoredState->GetMemories().Num(), 3);
    TestTrue(TEXT("Mission failure memory survives snapshot"), RestoredState->HasMemory(TEXT("Quest.Failed")));
    TestTrue(TEXT("Public shame rumor prefix survives snapshot"), RestoredState->HasMemoryPrefix(TEXT("Mission.Failure.")));
    TestTrue(TEXT("Unique mission rumor survives snapshot"), RestoredState->HasMemory(TEXT("Mission.Failure.Quest.Escort")));
    TestTrue(TEXT("Recovery removes the mission-specific rumor"), RestoredState->RemoveMemory(TEXT("Mission.Failure.Quest.Escort")));
    TestFalse(TEXT("Repaired rumor no longer remains in memory"), RestoredState->HasMemory(TEXT("Mission.Failure.Quest.Escort")));
    TestTrue(TEXT("Hunger survives snapshot"), FMath::IsNearlyEqual(RestoredState->GetHunger(), OriginalState->GetHunger()));
    TestTrue(TEXT("Energy survives snapshot"), FMath::IsNearlyEqual(RestoredState->GetEnergy(), OriginalState->GetEnergy()));

    UOmnisQuestSubsystem* OriginalQuests = NewObject<UOmnisQuestSubsystem>();
    UOmnisQuestSubsystem* RestoredQuests = NewObject<UOmnisQuestSubsystem>();
    TestNotNull(TEXT("Original quest subsystem"), OriginalQuests);
    TestNotNull(TEXT("Restored quest subsystem"), RestoredQuests);
    if (!OriginalQuests || !RestoredQuests)
    {
        return false;
    }

    TArray<FOmnisQuestProgressSnapshot> QuestSnapshots;
    FOmnisQuestProgressSnapshot FailedQuest;
    FailedQuest.QuestId = TEXT("Quest.Escort");
    FailedQuest.State = EOmnisQuestState::Failed;
    FailedQuest.Attempts = 2;
    FailedQuest.RecoveryQuestId = TEXT("Quest.RepairReputation");
    QuestSnapshots.Add(FailedQuest);
    OriginalQuests->ImportProgress(QuestSnapshots);
    RestoredQuests->ImportProgress(OriginalQuests->ExportProgress());

    TestTrue(
        TEXT("Failed quest state survives snapshot"),
        RestoredQuests->GetQuestState(TEXT("Quest.Escort")) == EOmnisQuestState::Failed);
    TestEqual(TEXT("Quest attempt count survives snapshot"), RestoredQuests->GetQuestAttemptCount(TEXT("Quest.Escort")), 2);
    TestTrue(
        TEXT("Recovery quest link survives snapshot"),
        RestoredQuests->GetRecoveryQuestId(TEXT("Quest.Escort")) == FName(TEXT("Quest.RepairReputation")));

    UOmnisConsequenceSubsystem* OriginalConsequences =
        NewObject<UOmnisConsequenceSubsystem>();
    UOmnisConsequenceSubsystem* RestoredConsequences =
        NewObject<UOmnisConsequenceSubsystem>();
    TestNotNull(TEXT("Original consequence subsystem"), OriginalConsequences);
    TestNotNull(TEXT("Restored consequence subsystem"), RestoredConsequences);
    if (!OriginalConsequences || !RestoredConsequences)
    {
        return false;
    }

    OriginalConsequences->ApplyConsequence(TEXT("Mission.Failure.PublicShame"));
    RestoredConsequences->ImportConsequences(OriginalConsequences->ExportConsequences());
    TestTrue(
        TEXT("Active consequence survives snapshot"),
        RestoredConsequences->HasConsequence(TEXT("Mission.Failure.PublicShame")));

    UOmnisSaveGame* Save = NewObject<UOmnisSaveGame>();
    TestNotNull(TEXT("Save object"), Save);
    if (!Save)
    {
        return false;
    }
    Save->SchemaVersion = 3;
    Save->QuestProgress = RestoredQuests->ExportProgress();
    Save->ActiveConsequences = RestoredConsequences->ExportConsequences();
    FOmnisSavedCharacterState Character;
    Character.PersistentId = TEXT("NPC.Test");
    Character.LifeState = RestoredState->ExportSnapshot();

    UOmnisInventoryComponent* OriginalInventory = NewObject<UOmnisInventoryComponent>();
    UOmnisInventoryComponent* RestoredInventory = NewObject<UOmnisInventoryComponent>();
    TestNotNull(TEXT("Original inventory"), OriginalInventory);
    TestNotNull(TEXT("Restored inventory"), RestoredInventory);
    if (!OriginalInventory || !RestoredInventory)
    {
        return false;
    }

    TestTrue(TEXT("Server inventory add"), OriginalInventory->AddItem(TEXT("Item.Potion"), 3));
    TestTrue(TEXT("Second item add"), OriginalInventory->AddItem(TEXT("Item.Key"), 1));
    TestFalse(TEXT("Inventory rejects invalid empty item IDs"), OriginalInventory->AddItem(NAME_None, 1));
    TestFalse(TEXT("Inventory rejects removal beyond owned quantity"), OriginalInventory->RemoveItem(TEXT("Item.Potion"), 99));
    RestoredInventory->ImportItems(OriginalInventory->ExportItems());
    TestEqual(TEXT("Inventory stack survives snapshot"), RestoredInventory->GetQuantity(TEXT("Item.Potion")), 3);
    TestEqual(TEXT("Unique inventory item survives snapshot"), RestoredInventory->GetQuantity(TEXT("Item.Key")), 1);

    UOmnisEquipmentComponent* OriginalEquipment = NewObject<UOmnisEquipmentComponent>();
    UOmnisEquipmentComponent* RestoredEquipment = NewObject<UOmnisEquipmentComponent>();
    TestNotNull(TEXT("Original equipment"), OriginalEquipment);
    TestNotNull(TEXT("Restored equipment"), RestoredEquipment);
    if (!OriginalEquipment || !RestoredEquipment)
    {
        return false;
    }

    TArray<FOmnisEquipmentEntry> EquipmentSnapshot;
    FOmnisEquipmentEntry EquippedItem;
    EquippedItem.Slot = TEXT("Slot.Primary");
    EquippedItem.ItemId = TEXT("Item.Sword");
    EquipmentSnapshot.Add(EquippedItem);
    OriginalEquipment->ImportEquipment(EquipmentSnapshot);
    RestoredEquipment->ImportEquipment(OriginalEquipment->ExportEquipment());
    TestTrue(TEXT("Equipped item survives snapshot"),
        RestoredEquipment->GetEquipped(TEXT("Slot.Primary")) == FName(TEXT("Item.Sword")));

    UOmnisAbilityComponent* OriginalAbilities = NewObject<UOmnisAbilityComponent>();
    UOmnisAbilityComponent* RestoredAbilities = NewObject<UOmnisAbilityComponent>();
    TestNotNull(TEXT("Original abilities"), OriginalAbilities);
    TestNotNull(TEXT("Restored abilities"), RestoredAbilities);
    if (!OriginalAbilities || !RestoredAbilities)
    {
        return false;
    }

    OriginalAbilities->SetCooldown(TEXT("Ability.Dash"), 15.0f);
    RestoredAbilities->ImportCooldowns(OriginalAbilities->ExportCooldowns());
    TestTrue(TEXT("Ability cooldown survives snapshot"),
        RestoredAbilities->IsOnCooldown(TEXT("Ability.Dash")));

    Character.InventoryItems = RestoredInventory->ExportItems();
    Character.EquippedItems = RestoredEquipment->ExportEquipment();
    Character.AbilityCooldowns = RestoredAbilities->ExportCooldowns();
    Save->Characters.Add(Character);

    TestEqual(TEXT("Save schema version"), Save->SchemaVersion, 3);
    TestEqual(TEXT("Saved quest count"), Save->QuestProgress.Num(), 1);
    TestEqual(TEXT("Saved character count"), Save->Characters.Num(), 1);
    TestEqual(TEXT("Saved inventory entries"), Save->Characters[0].InventoryItems.Num(), 2);
    TestEqual(TEXT("Saved equipped slots"), Save->Characters[0].EquippedItems.Num(), 1);
    TestEqual(TEXT("Saved cooldowns"), Save->Characters[0].AbilityCooldowns.Num(), 1);

    return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisWorldCalendarBoundaries,
    "Omnis.Time.CalendarBoundaries",
    EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter)

bool FOmnisWorldCalendarBoundaries::RunTest(const FString& Parameters)
{
    FOmnisWorldCalendarDate Date = UOmnisTimeSubsystem::BuildCalendarDate(0.0f);
    TestEqual(TEXT("Calendar starts in year one"), Date.Year, 1);
    TestTrue(TEXT("Calendar starts in spring"), Date.Season == EOmnisWorldSeason::Spring);
    TestEqual(TEXT("First day of year"), Date.DayOfYear, 1);
    TestEqual(TEXT("Monday is day zero"), Date.DayOfWeek, 0);
    TestEqual(TEXT("Midnight hour"), Date.Hour, 0);

    Date = UOmnisTimeSubsystem::BuildCalendarDate(60.0f);
    TestEqual(TEXT("Sixty simulated seconds advance one game hour"), Date.Hour, 1);

    Date = UOmnisTimeSubsystem::BuildCalendarDate(24.0f * 60.0f);
    TestEqual(TEXT("A full game day advances the date"), Date.DayOfYear, 2);
    TestEqual(TEXT("The weekday advances"), Date.DayOfWeek, 1);

    Date = UOmnisTimeSubsystem::BuildCalendarDate(30.0f * 24.0f * 60.0f);
    TestTrue(TEXT("Thirty days advance the season"), Date.Season == EOmnisWorldSeason::Summer);
    TestEqual(TEXT("First day of summer"), Date.DayOfSeason, 1);

    Date = UOmnisTimeSubsystem::BuildCalendarDate(120.0f * 24.0f * 60.0f);
    TestEqual(TEXT("Four seasons advance the year"), Date.Year, 2);
    TestTrue(TEXT("New year returns to spring"), Date.Season == EOmnisWorldSeason::Spring);
    TestEqual(TEXT("New year begins at day one"), Date.DayOfYear, 1);
    return true;
}

#endif