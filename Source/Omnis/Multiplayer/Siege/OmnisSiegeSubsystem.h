#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisSiegeSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisSiegeState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName TerritoryId;

    UPROPERTY(BlueprintReadOnly)
    FName AttackerId;

    UPROPERTY(BlueprintReadOnly)
    bool bActive = false;
};

UCLASS()
class OMNIS_API UOmnisSiegeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Siege")
    bool StartSiege(FName TerritoryId, FName AttackerId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Siege")
    void EndSiege();

    UFUNCTION(BlueprintPure, Category="Omnis|Siege")
    FOmnisSiegeState GetSiege() const { return State; }

private:
    UPROPERTY()
    FOmnisSiegeState State;
};
