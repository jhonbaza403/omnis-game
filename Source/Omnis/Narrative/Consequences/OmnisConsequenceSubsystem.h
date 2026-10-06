#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisConsequenceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisConsequenceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void ApplyConsequence(FName ConsequenceId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void RemoveConsequence(FName ConsequenceId);

    UFUNCTION(BlueprintPure, Category="Omnis|Narrative")
    bool HasConsequence(FName ConsequenceId) const;

private:
    UPROPERTY()
    TSet<FName> ActiveConsequences;
};
