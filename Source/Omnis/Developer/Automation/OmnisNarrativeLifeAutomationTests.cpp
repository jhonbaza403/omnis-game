#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Data/Quests/OmnisQuestDefinition.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Narrative/Quests/OmnisQuestSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisCharacterLifeStateAutomation,
    "Omnis.Narrative.CharacterLifeState",
    EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter)

bool FOmnisCharacterLifeStateAutomation::RunTest(const FString& Parameters)
{
    UOmnisCharacterStateComponent* State = NewObject<UOmnisCharacterStateComponent>();
    TestNotNull(TEXT("Character state component"), State);
    if (!State)
    {
        return false;
    }

    State->SimulateNeeds(10.0f, EOmnisCharacterLifeGoal::Work, false);
    TestTrue(TEXT("Hunger increases over time"), State->GetHunger() > 0.0f);
    TestTrue(TEXT("Social need increases over time"), State->GetSocialNeed() > 0.0f);
    TestTrue(TEXT("Energy decreases while active"), State->GetEnergy() < 100.0f);

    State->SimulateNeeds(10.0f, EOmnisCharacterLifeGoal::Rest, true);
    TestEqual(TEXT("Rest restores energy"), State->GetEnergy(), 100.0f);

    State->SimulateNeeds(10.0f, EOmnisCharacterLifeGoal::Eat, true);
    TestTrue(TEXT("Eating reduces hunger"), State->GetHunger() < 0.35f);

    State->SetRelationship(TEXT("Player.Test"), -500);
    TestEqual(TEXT("Relationships are bounded"), State->GetRelationship(TEXT("Player.Test")), -100);
    State->AdjustRelationship(TEXT("Player.Test"), 25);
    TestEqual(TEXT("Relationship changes are bounded and incremental"), State->GetRelationship(TEXT("Player.Test")), -75);

    for (int32 Index = 0; Index < 40; ++Index)
    {
        State->RecordMemory(FName(*FString::Printf(TEXT("Event.%d"), Index)), TEXT("Player.Test"), 10);
    }
    TestEqual(TEXT("Memory history is bounded"), State->GetMemories().Num(), 32);

    State->SetAlive(false);
    State->SimulateNeeds(100.0f, EOmnisCharacterLifeGoal::Rest, true);
    TestFalse(TEXT("Dead character remains dead"), State->IsAlive());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FOmnisQuestFailureAutomation,
    "Omnis.Narrative.QuestFailureConsequences",
    EAutomationTestFlags::ApplicationContextMask |
    EAutomationTestFlags::SmokeFilter)

bool FOmnisQuestFailureAutomation::RunTest(const FString& Parameters)
{
    UOmnisQuestSubsystem* Quests = NewObject<UOmnisQuestSubsystem>();
    UOmnisQuestDefinition* Definition = NewObject<UOmnisQuestDefinition>();
    TestNotNull(TEXT("Quest subsystem"), Quests);
    TestNotNull(TEXT("Quest definition"), Definition);
    if (!Quests || !Definition)
    {
        return false;
    }

    Definition->QuestId = TEXT("Quest.Automation.FailedEscort");
    Definition->RecoveryQuestId = TEXT("Quest.Automation.Recovery");
    Definition->FailureConsequenceId = TEXT("Mission.Failure.PublicShame");
    Definition->FailureReputationDelta = -10;
    Definition->FailureWorldStateKey = TEXT("World.MissionsFailed");
    Definition->FailureWorldStateDelta = 1;

    TestTrue(TEXT("Quest starts"), Quests->StartQuest(Definition));
    TestFalse(TEXT("Active quest cannot start twice"), Quests->StartQuest(Definition));
    TestTrue(TEXT("Active quest can fail"), Quests->FailQuest(Definition, TEXT("Quest.Failed.TimerExpired")));
    TestTrue(TEXT("Failed state is recorded"), Quests->GetQuestState(Definition->QuestId) == EOmnisQuestState::Failed);
    TestEqual(TEXT("Attempt count is tracked"), Quests->GetQuestAttemptCount(Definition->QuestId), 1);
    TestEqual(TEXT("Recovery quest is exposed"), Quests->GetRecoveryQuestId(Definition->QuestId), Definition->RecoveryQuestId);
    TestFalse(TEXT("Terminal failed quest cannot fail twice"), Quests->FailQuest(Definition, TEXT("Quest.Failed.Duplicate")));

    return true;
}

#endif
