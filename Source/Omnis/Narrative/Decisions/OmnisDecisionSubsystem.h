#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisDecisionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOmnisDecisionRecorded, FName, DecisionId, int32, Option);

UCLASS()
class OMNIS_API UOmnisDecisionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void RecordDecision(FName DecisionId, int32 Option);

    UFUNCTION(BlueprintPure, Category="Omnis|Narrative")
    int32 GetDecision(FName DecisionId) const;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Narrative")
    FOnOmnisDecisionRecorded OnDecisionRecorded;

private:
    UPROPERTY()
    TMap<FName, int32> Decisions;
};
