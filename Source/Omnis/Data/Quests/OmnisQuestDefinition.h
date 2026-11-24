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

    // Only named witnesses receive the personal memory/relationship update.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    TArray<FName> WitnessCharacterIds;

    // Optional alternate quest that lets players repair the damage.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest|Failure")
    FName RecoveryQuestId;
};
