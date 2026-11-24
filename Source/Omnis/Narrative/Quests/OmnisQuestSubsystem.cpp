#include "Narrative/Quests/OmnisQuestSubsystem.h"
#include "Multiplayer/World/OmnisReplicatedWorldStateActor.h"

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

namespace
{
    FString BuildMissionFailureRumorPrefix(FName QuestId)
    {
        return QuestId.IsNone()
            ? FString()
            : FString::Printf(TEXT("Mission.Failure.%s.Attempt."), *QuestId.ToString());
    }

    FName BuildMissionFailureRumorId(FName QuestId, int32 AttemptNumber)
    {
        const FString Prefix = BuildMissionFailureRumorPrefix(QuestId);
        return Prefix.IsEmpty()
            ? NAME_None
            : FName(*FString::Printf(TEXT("%s%d"), *Prefix, FMath::Max(1, AttemptNumber)));
    }
}

namespace
{
    void RefreshNarrativeReplication(UWorld* World)
    {
        if (!World)
        {
            return;
        }
        for (TActorIterator<AOmnisReplicatedWorldStateActor> It(World); It; ++It)
        {
            It->RefreshNarrativeState();
            break;
        }
    }
}

void UOmnisQuestSubsystem::SetQuestState(FName QuestId, EOmnisQuestState State)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
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
    RefreshNarrativeReplication(GetWorld());
}

EOmnisQuestState UOmnisQuestSubsystem::GetQuestState(FName QuestId) const
{
    const EOmnisQuestState* State = QuestStates.Find(QuestId);
    return State ? *State : EOmnisQuestState::Inactive;
}

bool UOmnisQuestSubsystem::StartQuest(const UOmnisQuestDefinition* Definition)
{
    if (!Definition || Definition->QuestId.IsNone() ||
        (GetWorld() && GetWorld()->GetNetMode() == NM_Client))
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
        (GetWorld() && GetWorld()->GetNetMode() == NM_Client) ||
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

    ApplyCompletionOutcome(Definition);
    RefreshNarrativeReplication(GetWorld());
    return true;
}

bool UOmnisQuestSubsystem::FailQuest(
    const UOmnisQuestDefinition* Definition,
    FName FailureReason)
{
    if (!Definition || Definition->QuestId.IsNone() ||
        (GetWorld() && GetWorld()->GetNetMode() == NM_Client) ||
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
    RefreshNarrativeReplication(GetWorld());
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
    UWorld* World = GetWorld();
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

    if (World && !Definition->FailureWorldStateKey.IsNone() &&
        Definition->FailureWorldStateDelta != 0)
    {
        if (UOmnisWorldStateSubsystem* WorldState =
            World->GetSubsystem<UOmnisWorldStateSubsystem>())
        {
            WorldState->IncrementState(
                Definition->FailureWorldStateKey,
                Definition->FailureWorldStateDelta);
        }
    }

    if (World)
    {
        const FName RumorEventId = BuildMissionFailureRumorId(
            Definition->QuestId,
            GetQuestAttemptCount(Definition->QuestId));
        const float ConfiguredWitnessRadius = FMath::IsFinite(Definition->FailureWitnessRadius)
            ? Definition->FailureWitnessRadius
            : 0.0f;
        const float WitnessRadius = FMath::Clamp(
            ConfiguredWitnessRadius,
            0.0f,
            10000.0f);
        const float WitnessRadiusSquared = FMath::Square(WitnessRadius);

        for (TActorIterator<AOmnisNPCCharacter> It(World); It; ++It)
        {
            AOmnisNPCCharacter* NPC = *It;
            if (!NPC || !NPC->CharacterState || !NPC->CharacterState->IsAlive())
            {
                continue;
            }

            const bool bExplicitWitness =
                Definition->WitnessCharacterIds.Contains(NPC->CharacterId) ||
                Definition->WitnessCharacterIds.Contains(NPC->GetPersistentId());

            bool bNearbyWitness = false;
            if (Definition->bFailureIsPublic && WitnessRadius > 0.0f)
            {
                for (TActorIterator<AOmnisPlayerCharacter> PlayerIt(World); PlayerIt; ++PlayerIt)
                {
                    const AOmnisPlayerCharacter* Player = *PlayerIt;
                    if (!Player || (Player->CharacterState && !Player->CharacterState->IsAlive()))
                    {
                        continue;
                    }

                    if (FVector::DistSquared(
                        NPC->GetActorLocation(),
                        Player->GetActorLocation()) <= WitnessRadiusSquared)
                    {
                        bNearbyWitness = true;
                        break;
                    }
                }
            }

            if (!bExplicitWitness && !bNearbyWitness)
            {
                continue;
            }

            NPC->CharacterState->RecordMemory(
                FailureReason,
                Definition->QuestId,
                100);
            if (!RumorEventId.IsNone())
            {
                NPC->CharacterState->RecordMemory(
                    RumorEventId,
                    Definition->FailureReputationTargetId,
                    100);
            }
            NPC->CharacterState->AdjustRelationship(
                Definition->FailureReputationTargetId,
                Definition->FailureRelationshipDelta);
        }

        for (TActorIterator<AOmnisPlayerCharacter> It(World); It; ++It)
        {
            AOmnisPlayerCharacter* Player = *It;
            if (!Player || !Player->CharacterState)
            {
                continue;
            }

            Player->CharacterState->RecordMemory(
                FailureReason,
                Definition->QuestId,
                80);
            if (!RumorEventId.IsNone())
            {
                Player->CharacterState->RecordMemory(
                    RumorEventId,
                    Definition->FailureReputationTargetId,
                    80);
            }
        }
    }

    if (UOmnisEventSubsystem* Events =
        GameInstance->GetSubsystem<UOmnisEventSubsystem>())
    {
        Events->RaiseEvent(
            TEXT("Quest.Failed"),
            GetQuestAttemptCount(Definition->QuestId));
    }
}

TArray<FOmnisQuestProgressSnapshot> UOmnisQuestSubsystem::ExportProgress() const
{
    TArray<FOmnisQuestProgressSnapshot> Result;
    Result.Reserve(QuestStates.Num());

    for (const TPair<FName, EOmnisQuestState>& Pair : QuestStates)
    {
        FOmnisQuestProgressSnapshot Snapshot;
        Snapshot.QuestId = Pair.Key;
        Snapshot.State = Pair.Value;
        Snapshot.Attempts = QuestAttempts.FindRef(Pair.Key);
        Snapshot.RecoveryQuestId = RecoveryQuestByFailedQuest.FindRef(Pair.Key);
        Result.Add(Snapshot);
    }

    Result.Sort([](const FOmnisQuestProgressSnapshot& A, const FOmnisQuestProgressSnapshot& B)
    {
        return A.QuestId.Compare(B.QuestId) < 0;
    });
    return Result;
}

void UOmnisQuestSubsystem::ImportProgress(
    const TArray<FOmnisQuestProgressSnapshot>& Snapshots)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    QuestStates.Reset();
    QuestAttempts.Reset();
    RecoveryQuestByFailedQuest.Reset();

    for (const FOmnisQuestProgressSnapshot& Snapshot : Snapshots)
    {
        if (Snapshot.QuestId.IsNone() ||
            static_cast<uint8>(Snapshot.State) > static_cast<uint8>(EOmnisQuestState::Failed))
        {
            continue;
        }

        QuestStates.Add(Snapshot.QuestId, Snapshot.State);
        QuestAttempts.Add(Snapshot.QuestId, FMath::Max(0, Snapshot.Attempts));
        if (!Snapshot.RecoveryQuestId.IsNone())
        {
            RecoveryQuestByFailedQuest.Add(Snapshot.QuestId, Snapshot.RecoveryQuestId);
        }
    }
}

void UOmnisQuestSubsystem::ApplyCompletionOutcome(
    const UOmnisQuestDefinition* Definition)
{
    if (!Definition)
    {
        return;
    }

    UGameInstance* GameInstance = GetGameInstance();
    UWorld* World = GetWorld();
    if (!GameInstance)
    {
        return;
    }

    if (UOmnisConsequenceSubsystem* Consequences =
        GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
    {
        for (const FName ConsequenceId : Definition->ConsequencesToRemoveOnCompletion)
        {
            Consequences->RemoveConsequence(ConsequenceId);
        }
    }

    if (!Definition->RecoveryReputationTargetId.IsNone() &&
        Definition->RecoveryReputationDelta != 0)
    {
        if (UOmnisReputationSubsystem* Reputation =
            GameInstance->GetSubsystem<UOmnisReputationSubsystem>())
        {
            Reputation->AddReputation(
                Definition->RecoveryReputationTargetId,
                Definition->RecoveryReputationDelta);
        }
    }

    if (!Definition->RecoveryFactionId.IsNone() &&
        Definition->RecoveryFactionStandingDelta != 0)
    {
        if (UOmnisFactionSubsystem* Factions =
            GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
        {
            Factions->AdjustStanding(
                Definition->RecoveryFactionId,
                Definition->RecoveryFactionStandingDelta);
        }
    }

    if (World && !Definition->RecoveryWorldStateKey.IsNone() &&
        Definition->RecoveryWorldStateDelta != 0)
    {
        if (UOmnisWorldStateSubsystem* WorldState =
            World->GetSubsystem<UOmnisWorldStateSubsystem>())
        {
            const int32 Current = WorldState->GetState(
                Definition->RecoveryWorldStateKey, 0);
            WorldState->SetState(
                Definition->RecoveryWorldStateKey,
                FMath::Max(0, Current + Definition->RecoveryWorldStateDelta));
        }
    }

    if (!World)
    {
        return;
    }

    // A recovery quest repairs all remembered attempts linked to this recovery asset.
    TArray<FName> FailedQuestIds;
    for (const TPair<FName, FName>& Pair : RecoveryQuestByFailedQuest)
    {
        if (Pair.Value == Definition->QuestId)
        {
            FailedQuestIds.Add(Pair.Key);
        }
    }

    for (TActorIterator<AOmnisNPCCharacter> It(World); It; ++It)
    {
        AOmnisNPCCharacter* NPC = *It;
        if (!NPC || !NPC->CharacterState)
        {
            continue;
        }

        int32 RepairedRumorCount = 0;
        for (const FName FailedQuestId : FailedQuestIds)
        {
            const FString RumorPrefix = BuildMissionFailureRumorPrefix(FailedQuestId);
            const int32 RemovedCount =
                NPC->CharacterState->RemoveMemoriesByPrefix(RumorPrefix);
            if (RemovedCount > 0)
            {
                NPC->CharacterState->RecordMemory(
                    TEXT("Quest.RecoveryCompleted"),
                    FailedQuestId,
                    80);
                RepairedRumorCount += RemovedCount;
            }
        }

        const bool bConfiguredRecoveryWitness =
            Definition->RecoveryWitnessCharacterIds.Contains(NPC->CharacterId) ||
            Definition->RecoveryWitnessCharacterIds.Contains(NPC->GetPersistentId());

        if (bConfiguredRecoveryWitness && RepairedRumorCount == 0)
        {
            NPC->CharacterState->RecordMemory(
                TEXT("Quest.RecoveryCompleted"),
                Definition->QuestId,
                80);
            RepairedRumorCount = 1;
        }

        if (RepairedRumorCount > 0 && Definition->RecoveryRelationshipDelta != 0)
        {
            const FName SubjectId = Definition->RecoveryReputationTargetId;
            const int32 Previous = NPC->CharacterState->GetRelationship(SubjectId);
            const int64 Desired = static_cast<int64>(Previous) +
                static_cast<int64>(FMath::Clamp(Definition->RecoveryRelationshipDelta, -100, 100)) *
                static_cast<int64>(RepairedRumorCount);
            const int32 BoundedDesired = static_cast<int32>(FMath::Clamp<int64>(Desired, -100, 100));
            NPC->CharacterState->AdjustRelationship(SubjectId, BoundedDesired - Previous);
        }
    }

    for (TActorIterator<AOmnisPlayerCharacter> It(World); It; ++It)
    {
        AOmnisPlayerCharacter* Player = *It;
        if (!Player || !Player->CharacterState)
        {
            continue;
        }

        for (const FName FailedQuestId : FailedQuestIds)
        {
            const FString RumorPrefix = BuildMissionFailureRumorPrefix(FailedQuestId);
            if (Player->CharacterState->RemoveMemoriesByPrefix(RumorPrefix) > 0)
            {
                Player->CharacterState->RecordMemory(
                    TEXT("Quest.RecoveryCompleted"),
                    FailedQuestId,
                    80);
            }
        }
    }
}
