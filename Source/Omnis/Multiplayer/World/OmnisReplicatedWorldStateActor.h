#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Narrative/Quests/OmnisQuestSubsystem.h"
#include "OmnisReplicatedWorldStateActor.generated.h"

USTRUCT(BlueprintType)
struct FOmnisReplicatedQuestState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    FName QuestId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    EOmnisQuestState State = EOmnisQuestState::Inactive;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    int32 Attempts = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    FName RecoveryQuestId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    FName FailureConsequenceId;
};

USTRUCT(BlueprintType)
struct FOmnisReplicatedWorldValue
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    FName Key;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|World")
    int32 Value = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOmnisNarrativeStateChanged);

UCLASS()
class OMNIS_API AOmnisReplicatedWorldStateActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisReplicatedWorldStateActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    int32 GetCycleIndex() const { return CycleIndex; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    int32 GetWorldRevision() const { return WorldRevision; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    FString GetEraName() const { return EraName; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    TArray<FOmnisReplicatedQuestState> GetQuestStates() const { return QuestStates; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    TArray<FName> GetActiveConsequences() const { return ActiveConsequences; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    TArray<FOmnisReplicatedWorldValue> GetReputationValues() const { return ReputationValues; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    TArray<FOmnisReplicatedWorldValue> GetFactionStandings() const { return FactionStandings; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    TArray<FOmnisReplicatedWorldValue> GetWorldValues() const { return WorldValues; }

    void RefreshFromBackend();
    void RefreshNarrativeState();

    UPROPERTY(BlueprintAssignable, Category="Omnis|World")
    FOnOmnisNarrativeStateChanged OnNarrativeStateChanged;

private:
    UFUNCTION()
    void OnRep_NarrativeState();

    void ApplyBackendResponse(const FString& ResponseBody);

    UPROPERTY(Replicated)
    int32 CycleIndex = 0;

    UPROPERTY(Replicated)
    int32 WorldRevision = 0;

    UPROPERTY(Replicated)
    FString EraName;

    UPROPERTY(ReplicatedUsing=OnRep_NarrativeState)
    TArray<FOmnisReplicatedQuestState> QuestStates;

    UPROPERTY(ReplicatedUsing=OnRep_NarrativeState)
    TArray<FName> ActiveConsequences;

    UPROPERTY(ReplicatedUsing=OnRep_NarrativeState)
    TArray<FOmnisReplicatedWorldValue> ReputationValues;

    UPROPERTY(ReplicatedUsing=OnRep_NarrativeState)
    TArray<FOmnisReplicatedWorldValue> FactionStandings;

    UPROPERTY(ReplicatedUsing=OnRep_NarrativeState)
    TArray<FOmnisReplicatedWorldValue> WorldValues;
};
