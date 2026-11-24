#include "Narrative/Quests/OmnisQuestSubsystem.h"

#include "Core/Events/OmnisEventSubsystem.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Data/Quests/OmnisQuestDefinition.h"
#include "EngineUtils.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "AI/NPC/OmnisNPCCharacter.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"
#include "Narrative/Factions/OmnisFactionSubsystem.h"
#include "Narrative/Reputation/OmnisReputationSubsystem.h"
#include "World/WorldState/OmnisWorldStateSubsystem.h"

void UOmnisQuestSubsystem::SetQuestState(FName QuestId, EOmnisQuestState State)
{
    SetQuestStateInternal(QuestId, State, NAME_None);
}

void UOmnisQuestSubsystem::SetQuestStateInternal(
    FName QuestId,
    EOmnisQuestState State,
    FName Reason)
{
    if (QuestId.IsNone())
    {
        return;
    }

    QuestStates.Add(QuestId, State);
    OnQuestStateChanged.Broadcast(QuestId, State, Reason);
}

EOmnisQuestState UOmnisQuestSubsystem::GetQuestState(FName QuestId) const
{
    const EOmnisQuestState* State = QuestStates.Find(QuestId);
    return State ? *State : EOmnisQuestState::Inactive;
}

bool UOmnisQuestSubsystem::StartQuest(const UOmnisQuestDefinition* Definition)
{
    if (!Definition || Definition->QuestId.IsNone())
    {
        return false;
    }

    const EOmnisQuestState CurrentState = GetQuestState(Definition->QuestId);
    if (CurrentState == EOmnisQuestState::Active ||
        CurrentState == EOmnisQuestState::Completed)
    {
        return false;
    }

    QuestAttempts.FindOrAdd(Definition->QuestId)++;
    SetQuestStateInternal(
        Definition->QuestId,
        EOmnisQuestState::Active,
        TEXT("Quest.Started"));
    return true;
}

bool UOmnisQuestSubsystem::CompleteQuest(const UOmnisQuestDefinition* Definition)
{
    if (!Definition || Definition->QuestId.IsNone() ||
        GetQuestState(Definition->QuestId) != EOmnisQuestState::Active)
    {
        return false;
    }

    SetQuestStateInternal(
        Definition->QuestId,
        EOmnisQuestState::Completed,
        TEXT("Quest.Completed"));

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisProgressionSubsystem* Progression =
            GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
        {
            Progression->AddExperience(FMath::Max(0, Definition->ExperienceReward));
        }

        if (UOmnisEventSubsystem* Events =
            GameInstance->GetSubsystem<UOmnisEventSubsystem>())
        {
            Events->RaiseEvent(TEXT("Quest.Completed"), 1);
        }
    }

    return true;
}

bool UOmnisQuestSubsystem::FailQuest(
    const UOmnisQuestDefinition* Definition,
    FName FailureReason)
{
    if (!Definition || Definition->QuestId.IsNone() ||
        GetQuestState(Definition->QuestId) != EOmnisQuestState::Active)
    {
        return false;
    }

    const FName ResolvedReason = FailureReason.IsNone()
        ? FName(TEXT("Quest.Failed"))
        : FailureReason;

    SetQuestStateInternal(
        Definition->QuestId,
        EOmnisQuestState::Failed,
        ResolvedReason);

    if (!Definition->RecoveryQuestId.IsNone())
    {
        RecoveryQuestByFailedQuest.Add(
            Definition->QuestId,
            Definition->RecoveryQuestId);
    }

    ApplyFailureOutcome(Definition, ResolvedReason);
    return true;
}

int32 UOmnisQuestSubsystem::GetQuestAttemptCount(FName QuestId) const
{
    return QuestAttempts.FindRef(QuestId);
}

FName UOmnisQuestSubsystem::GetRecoveryQuestId(FName FailedQuestId) const
{
    return RecoveryQuestByFailedQuest.FindRef(FailedQuestId);
}

void UOmnisQuestSubsystem::ApplyFailureOutcome(
    const UOmnisQuestDefinition* Definition,
    FName FailureReason)
{
    if (!Definition)
    {
        return;
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return;
    }

    if (!Definition->FailureConsequenceId.IsNone())
    {
        if (UOmnisConsequenceSubsystem* Consequences =
            GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
        {
            Consequences->ApplyConsequence(Definition->FailureConsequenceId);
        }
    }

    if (!Definition->FailureReputationTargetId.IsNone() &&
        Definition->FailureReputationDelta != 0)
    {
        if (UOmnisReputationSubsystem* Reputation =
            GameInstance->GetSubsystem<UOmnisReputationSubsystem>())
        {
            Reputation->AddReputation(
                Definition->FailureReputationTargetId,
                Definition->FailureReputationDelta);
        }
    }

    if (!Definition->FailureFactionId.IsNone() &&
        Definition->FailureFactionStandingDelta != 0)
    {
        if (UOmnisFactionSubsystem* Factions =
            GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
        {
            Factions->AdjustStanding(
                Definition->FailureFactionId,
                Definition->FailureFactionStandingDelta);
        }
    }

    if (!Definition->FailureWorldStateKey.IsNone() &&
        Definition->FailureWorldStateDelta != 0)
    {
        if (UWorld* World = GetWorld())
        {
            if (UOmnisWorldStateSubsystem* WorldState =
                World->GetSubsystem<UOmnisWorldStateSubsystem>())
            {
                WorldState->IncrementState(
                    Definition->FailureWorldStateKey,
                    Definition->FailureWorldStateDelta);
            }
        }
    }

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AOmnisNPCCharacter> It(World); It; ++It)
        {
            AOmnisNPCCharacter* NPC = *It;
            if (!NPC || !NPC->CharacterState ||
                !Definition->WitnessCharacterIds.Contains(NPC->CharacterId))
            {
                continue;
            }

            NPC->CharacterState->RecordMemory(
                FailureReason,
                Definition->QuestId,
                100);
            NPC->CharacterState->AdjustRelationship(
                Definition->FailureReputationTargetId,
                Definition->FailureRelationshipDelta);
        }

        for (TActorIterator<AOmnisPlayerCharacter> It(World); It; ++It)
        {
            if (It->CharacterState)
            {
                It->CharacterState->RecordMemory(
                    FailureReason,
                    Definition->QuestId,
                    80);
            }
        }
    }

    if (UOmnisEventSubsystem* Events =
        GameInstance->GetSubsystem<UOmnisEventSubsystem>())
    {
        Events->RaiseEvent(TEXT("Quest.Failed"), GetQuestAttemptCount(Definition->QuestId));
    }
}
