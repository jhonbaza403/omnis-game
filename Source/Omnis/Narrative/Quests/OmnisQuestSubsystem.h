#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisQuestSubsystem.generated.h"

class UOmnisQuestDefinition;

UENUM(BlueprintType)
enum class EOmnisQuestState : uint8
{
    Inactive,
    Active,
    Completed,
    Failed,
    Recovered
};

USTRUCT(BlueprintType)
struct FOmnisQuestProgressSnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Quest")
    FName QuestId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Quest")
    EOmnisQuestState State = EOmnisQuestState::Inactive;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Quest")
    int32 Attempts = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Quest")
    FName RecoveryQuestId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Quest")
    FName FailureConsequenceId;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOmnisQuestStateChanged,
    FName, QuestId,
    EOmnisQuestState, NewState,
    FName, Reason);

UCLASS()
class OMNIS_API UOmnisQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    void SetQuestState(FName QuestId, EOmnisQuestState State);

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    EOmnisQuestState GetQuestState(FName QuestId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    bool StartQuest(const UOmnisQuestDefinition* Definition);

    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    bool CompleteQuest(const UOmnisQuestDefinition* Definition);

    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    bool FailQuest(const UOmnisQuestDefinition* Definition, FName FailureReason);

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    int32 GetQuestAttemptCount(FName QuestId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    FName GetRecoveryQuestId(FName FailedQuestId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    FName GetFailureConsequenceId(FName FailedQuestId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    void RestoreFailureConsequences();

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    bool HasUnresolvedFailureUsingConsequence(FName ConsequenceId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    TArray<FOmnisQuestProgressSnapshot> ExportProgress() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    void ImportProgress(const TArray<FOmnisQuestProgressSnapshot>& Snapshots);

    UPROPERTY(BlueprintAssignable, Category="Omnis|Quest")
    FOmnisQuestStateChanged OnQuestStateChanged;

private:
    void SetQuestStateInternal(FName QuestId, EOmnisQuestState State, FName Reason);
    void ApplyFailureOutcome(const UOmnisQuestDefinition* Definition, FName FailureReason);
    void ApplyCompletionOutcome(const UOmnisQuestDefinition* Definition);

    UPROPERTY(SaveGame)
    TMap<FName, EOmnisQuestState> QuestStates;

    UPROPERTY(SaveGame)
    TMap<FName, int32> QuestAttempts;

    UPROPERTY(SaveGame)
    TMap<FName, FName> RecoveryQuestByFailedQuest;

    UPROPERTY(SaveGame)
    TMap<FName, FName> FailureConsequenceByFailedQuest;
};
