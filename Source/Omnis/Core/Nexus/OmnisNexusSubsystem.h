#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisNexusSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisNexusProgression
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Nexus")
    int32 PermanentUpgradePoints = 0;
};

UCLASS()
class OMNIS_API UOmnisNexusSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Nexus")
    void AddPermanentUpgradePoints(int32 Amount);

    UFUNCTION(BlueprintPure, Category="Omnis|Nexus")
    int32 GetPermanentUpgradePoints() const { return Progression.PermanentUpgradePoints; }

private:
    UPROPERTY()
    FOmnisNexusProgression Progression;
};
