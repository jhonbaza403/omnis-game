#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisRankedSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisRankedState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Ranked")
    int32 Rating = 1000;

    UPROPERTY(BlueprintReadOnly, Category="Ranked")
    int32 Wins = 0;

    UPROPERTY(BlueprintReadOnly, Category="Ranked")
    int32 Losses = 0;

    UPROPERTY(BlueprintReadOnly, Category="Ranked")
    int32 Division = 1;
};

UCLASS()
class OMNIS_API UOmnisRankedSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Competitive")
    void RecordMatchResult(bool bWon);

    UFUNCTION(BlueprintCallable, Category="Omnis|Competitive")
    void ApplySkillDelta(int32 Delta);

    UFUNCTION(BlueprintPure, Category="Omnis|Competitive")
    FOmnisRankedState GetRankedState() const { return State; }

    UFUNCTION(BlueprintPure, Category="Omnis|Competitive")
    FText GetDivisionName() const;

private:
    void RecalculateDivision();

    UPROPERTY()
    FOmnisRankedState State;
};
