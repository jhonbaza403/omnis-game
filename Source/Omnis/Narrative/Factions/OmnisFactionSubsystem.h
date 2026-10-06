#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisFactionSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisFactionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Factions")
    void AdjustStanding(FName FactionId, int32 Delta);

    UFUNCTION(BlueprintPure, Category="Omnis|Factions")
    int32 GetStanding(FName FactionId) const;

private:
    UPROPERTY()
    TMap<FName, int32> Standings;
};
