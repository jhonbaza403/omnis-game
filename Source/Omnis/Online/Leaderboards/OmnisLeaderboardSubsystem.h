#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisLeaderboardSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisLeaderboardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SubmitScore(FName BoardId, int32 Score);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    int32 GetScore(FName BoardId) const;

private:
    UPROPERTY()
    TMap<FName, int32> Scores;
};
