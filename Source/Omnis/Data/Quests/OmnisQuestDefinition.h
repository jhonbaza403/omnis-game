#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "OmnisQuestDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisQuestDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
    FName QuestId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
    FText Title;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Rewards")
    int32 ExperienceReward = 100;

    // A failed mission is authored as a world event, not just a Failed enum.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    FName FailureConsequenceId = TEXT("Mission.Failure.PublicShame");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    FName FailureReputationTargetId = TEXT("Player");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    int32 FailureReputationDelta = -10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    FName FailureFactionId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    int32 FailureFactionStandingDelta = -10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    FName FailureWorldStateKey = TEXT("World.MissionsFailed");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    int32 FailureWorldStateDelta = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    int32 FailureRelationshipDelta = -15;

    // Witness IDs may target an archetype CharacterId or an individual PersistentId.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    TArray<FName> WitnessCharacterIds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    FName RecoveryQuestId;

    // Recovery missions explicitly describe which consequences and penalties they repair.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    TArray<FName> ConsequencesToRemoveOnCompletion;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    FName RecoveryReputationTargetId = TEXT("Player");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    int32 RecoveryReputationDelta = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    FName RecoveryFactionId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    int32 RecoveryFactionStandingDelta = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    FName RecoveryWorldStateKey = TEXT("World.MissionsFailed");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    int32 RecoveryWorldStateDelta = -1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    int32 RecoveryRelationshipDelta = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Recovery")
    TArray<FName> RecoveryWitnessCharacterIds;
};
