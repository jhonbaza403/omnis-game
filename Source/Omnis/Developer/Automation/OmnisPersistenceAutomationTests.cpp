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
#include "World/Economy/OmnisEconomySubsystem.h"

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
    OriginalState->RecordMemory(TEXT("Mission.Failure.Quest.Escort.Attempt.1"), TEXT("Player"), 100);
    OriginalState->SimulateNeeds(10.0f, EOmnisCharacterLifeGoal::Work, false);
    const FOmnisCharacterLifeSnapshot LifeSnapshot = OriginalState->ExportSnapshot();
    RestoredState->ImportSnapshot(LifeSnapshot);

    TestEqual(TEXT("Relationship survives snapshot"), RestoredState->GetRelationship(TEXT("Player")), -65);
    TestEqual(TEXT("Memory count survives snapshot"), RestoredState->GetMemories().Num(), 3);
    TestTrue(TEXT("Mission failure memory survives snapshot"), RestoredState->HasMemory(TEXT("Quest.Failed")));
    TestTrue(TEXT("Public shame rumor prefix survives snapshot"), RestoredState->HasMemoryPrefix(TEXT("Mission.Failure.")));
    TestTrue(TEXT("Unique mission attempt rumor survives snapshot"), RestoredState->HasMemory(TEXT("Mission.Failure.Quest.Escort.Attempt.1")));
    TestEqual(
        TEXT("Recovery removes mission-specific attempt rumors"),
        RestoredState->RemoveMemoriesByPrefix(TEXT("Mission.Failure.Quest.Escort.Attempt.")),
        1);
    TestFalse(TEXT("Repaired attempt rumor no longer remains"),
        RestoredState->HasMemoryPrefix(TEXT("Mission.Failure.Quest.Escort.Attempt.")));
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
    FailedQuest.FailureConsequenceId = TEXT("Mission.Failure.PublicShame");
    QuestSnapshots.Add(FailedQuest);

    FOmnisQuestProgressSnapshot SecondFailedQuest;
    SecondFailedQuest.QuestId = TEXT("Quest.Sabotage");
    SecondFailedQuest.State = EOmnisQuestState::Failed;
    SecondFailedQuest.Attempts = 1;
    SecondFailedQuest.RecoveryQuestId = TEXT("Quest.RepairReputation");
    SecondFailedQuest.FailureConsequenceId = TEXT("Mission.Failure.PublicShame");
    QuestSnapshots.Add(SecondFailedQuest);

    FOmnisQuestProgressSnapshot RecoveredQuest;
    RecoveredQuest.QuestId = TEXT("Quest.Rescue");
    RecoveredQuest.State = EOmnisQuestState::Recovered;
    RecoveredQuest.Attempts = 1;
    QuestSnapshots.Add(RecoveredQuest);
    OriginalQuests->ImportProgress(QuestSnapshots);
    RestoredQuests->ImportProgress(OriginalQuests->ExportProgress());

    TestTrue(
        TEXT("Failed quest state survives snapshot"),
        RestoredQuests->GetQuestState(TEXT("Quest.Escort")) == EOmnisQuestState::Failed);
    TestEqual(TEXT("Quest attempt count survives snapshot"), RestoredQuests->GetQuestAttemptCount(TEXT("Quest.Escort")), 2);
    TestTrue(
        TEXT("Recovery quest link survives snapshot"),
        RestoredQuests->GetRecoveryQuestId(TEXT("Quest.Escort")) == FName(TEXT("Quest.RepairReputation")));
    TestTrue(
        TEXT("Per-quest failure consequence survives snapshot"),
        RestoredQuests->GetFailureConsequenceId(TEXT("Quest.Escort")) == FName(TEXT("Mission.Failure.PublicShame")));
    TestTrue(
        TEXT("Shared aggregate remains unresolved while either mission is failed"),
        RestoredQuests->HasUnresolvedFailureUsingConsequence(TEXT("Mission.Failure.PublicShame")));
    TestTrue(
        TEXT("Second mission retains the same aggregate mapping independently"),
        RestoredQuests->GetFailureConsequenceId(TEXT("Quest.Sabotage")) == FName(TEXT("Mission.Failure.PublicShame")));
    TestTrue(
        TEXT("Recovered state survives snapshot and is not counted as unresolved"),
        RestoredQuests->GetQuestState(TEXT("Quest.Rescue")) == EOmnisQuestState::Recovered);
    TestTrue(
        TEXT("Recovered quest does not rebuild a failure consequence"),
        RestoredQuests->GetFailureConsequenceId(TEXT("Quest.Rescue")).IsNone());

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
    Save->SchemaVersion = 6;
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

    TestEqual(TEXT("Save schema version"), Save->SchemaVersion, 6);
    TestEqual(TEXT("Saved quest count"), Save->QuestProgress.Num(), 3);
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


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisEconomySupplyDemandRoundTrip,
    "Omnis.Economy.SupplyDemandRoundTrip",
    EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter)

bool FOmnisEconomySupplyDemandRoundTrip::RunTest(const FString& Parameters)
{
    UOmnisEconomySubsystem* Economy = NewObject<UOmnisEconomySubsystem>();
    TestNotNull(TEXT("Economy subsystem"), Economy);
    if (!Economy)
    {
        return false;
    }

    const FName MarketId(TEXT("Market.Automation.Test"));
    const FName RegionId(TEXT("Region.Automation.Test"));
    const FName ItemId(TEXT("Item.Automation.Fuel"));
    const FName CurrencyId(TEXT("Currency.Automation"));
    const FName AccountId(TEXT("Account.Automation.Player"));

    TestTrue(
        TEXT("Register initial market item"),
        Economy->RegisterMarketCommodity(
            MarketId, RegionId, ItemId, CurrencyId, 10000, 100, 100));

    Economy->RegisterVenezuelaTriangleMarkets();
    const FName RegionalMarketId(TEXT("Market.Higuerote.Coastal"));
    const FName FuelItemId(TEXT("Item.Fuel"));
    const FName CoastalRouteId(TEXT("Route.Caucagua-RioChico-Higuerote"));
    const FOmnisMarketQuote FuelBeforeShock =
        Economy->GetQuote(RegionalMarketId, FuelItemId);
    TArray<FName> AffectedItems;
    AffectedItems.Add(FuelItemId);
    TestTrue(TEXT("Fuel quote exists before route closure"), FuelBeforeShock.bAvailable);
    TestEqual(
        TEXT("The route shock updates only linked markets that stock fuel"),
        Economy->ApplyRouteSupplyShock(
            CoastalRouteId, AffectedItems, -10, 10),
        2);
    const FOmnisMarketQuote FuelAfterShock =
        Economy->GetQuote(RegionalMarketId, FuelItemId);
    TestTrue(TEXT("A disrupted coastal route raises local fuel price"),
        FuelAfterShock.UnitPriceMinorUnits > FuelBeforeShock.UnitPriceMinorUnits);

    FOmnisMarketQuote Quote = Economy->GetQuote(MarketId, ItemId);
    TestTrue(TEXT("Initial quote exists"), Quote.bAvailable);
    TestEqual(TEXT("Balanced stock and demand use base price"), Quote.UnitPriceMinorUnits, 10000LL);
    TestEqual(TEXT("Balanced stock and demand use neutral index"), Quote.PriceIndexBasisPoints, 10000);

    TestTrue(
        TEXT("A supply shock can reduce available stock"),
        Economy->AdjustMarketConditions(MarketId, ItemId, -90, 0));
    Quote = Economy->GetQuote(MarketId, ItemId);
    TestEqual(TEXT("Supply shock updates stock"), Quote.StockQuantity, 10);
    TestTrue(TEXT("Scarcity raises the price"), Quote.UnitPriceMinorUnits > 10000LL);
    TestTrue(TEXT("Price index remains within configured ceiling"), Quote.PriceIndexBasisPoints <= 40000);

    // Re-registering a streamed market is idempotent and does not refill stock.
    TestTrue(
        TEXT("Repeated market registration remains valid"),
        Economy->RegisterMarketCommodity(
            MarketId, RegionId, ItemId, CurrencyId, 10000, 100, 100));
    TestEqual(TEXT("Repeated registration does not reset market stock"),
        Economy->GetQuote(MarketId, ItemId).StockQuantity, 10);

    Economy->SetAccountBalance(AccountId, CurrencyId, 250000);
    TestEqual(TEXT("Account balance uses integer minor units"),
        Economy->GetAccountBalance(AccountId, CurrencyId), 250000LL);

    const FOmnisEconomySnapshot Snapshot = Economy->ExportState();
    UOmnisEconomySubsystem* RestoredEconomy = NewObject<UOmnisEconomySubsystem>();
    TestNotNull(TEXT("Restored economy subsystem"), RestoredEconomy);
    if (!RestoredEconomy)
    {
        return false;
    }

    RestoredEconomy->ImportState(Snapshot);
    const FOmnisMarketQuote RestoredQuote = RestoredEconomy->GetQuote(MarketId, ItemId);
    TestEqual(TEXT("Market stock survives save snapshot"), RestoredQuote.StockQuantity, 10);
    TestEqual(TEXT("Market price survives save snapshot"),
        RestoredQuote.UnitPriceMinorUnits, Quote.UnitPriceMinorUnits);
    TestEqual(TEXT("Wallet balance survives save snapshot"),
        RestoredEconomy->GetAccountBalance(AccountId, CurrencyId), 250000LL);

    return true;
}


#endif