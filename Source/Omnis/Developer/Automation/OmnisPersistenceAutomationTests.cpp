#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Core/Save/OmnisSaveGame.h"
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
    OriginalState->SimulateNeeds(10.0f, EOmnisCharacterLifeGoal::Work, false);
    const FOmnisCharacterLifeSnapshot LifeSnapshot = OriginalState->ExportSnapshot();
    RestoredState->ImportSnapshot(LifeSnapshot);

    TestEqual(TEXT("Relationship survives snapshot"), RestoredState->GetRelationship(TEXT("Player")), -65);
    TestEqual(TEXT("Memory count survives snapshot"), RestoredState->GetMemories().Num(), 2);
    TestTrue(TEXT("Mission failure memory survives snapshot"), RestoredState->HasMemory(TEXT("Quest.Failed")));
    TestTrue(TEXT("Public shame rumor prefix survives snapshot"), RestoredState->HasMemoryPrefix(TEXT("Mission.Failure.")));
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
    Save->SchemaVersion = 2;
    Save->QuestProgress = RestoredQuests->ExportProgress();
    Save->ActiveConsequences = RestoredConsequences->ExportConsequences();
    FOmnisSavedCharacterState Character;
    Character.PersistentId = TEXT("NPC.Test");
    Character.LifeState = RestoredState->ExportSnapshot();
    Save->Characters.Add(Character);

    TestEqual(TEXT("Save schema version"), Save->SchemaVersion, 2);
    TestEqual(TEXT("Saved quest count"), Save->QuestProgress.Num(), 1);
    TestEqual(TEXT("Saved character count"), Save->Characters.Num(), 1);

    return true;
}

#endif