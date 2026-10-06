#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisTerritorySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisTerritoryClaim
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName TerritoryId;

    UPROPERTY(BlueprintReadOnly)
    FName OwnerId;
};

UCLASS()
class OMNIS_API UOmnisTerritorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Territory")
    bool ClaimTerritory(FName TerritoryId, FName OwnerId);

    UFUNCTION(BlueprintPure, Category="Omnis|Territory")
    FName GetOwner(FName TerritoryId) const;

private:
    UPROPERTY()
    TMap<FName, FName> Owners;
};
