#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisReputationSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisReputationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Reputation")
    void AddReputation(FName SubjectId, int32 Amount);

    UFUNCTION(BlueprintPure, Category="Omnis|Reputation")
    int32 GetReputation(FName SubjectId) const;

private:
    UPROPERTY()
    TMap<FName, int32> Reputation;
};
