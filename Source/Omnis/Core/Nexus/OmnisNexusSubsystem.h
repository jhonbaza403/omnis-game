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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Nexus")
    int32 PermanentVitalityLevel = 0;
};

UCLASS()
class OMNIS_API UOmnisNexusSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Nexus")
    void AddPermanentUpgradePoints(int32 Amount);

    UFUNCTION(BlueprintCallable, Category="Omnis|Nexus")
    bool PurchaseVitalityUpgrade();

    UFUNCTION(BlueprintCallable, Category="Omnis|Nexus")
    void RestoreProgression(int32 SavedPoints, int32 SavedVitalityLevel);

    UFUNCTION(BlueprintPure, Category="Omnis|Nexus")
    int32 GetPermanentUpgradePoints() const { return Progression.PermanentUpgradePoints; }

    UFUNCTION(BlueprintPure, Category="Omnis|Nexus")
    int32 GetPermanentVitalityLevel() const { return Progression.PermanentVitalityLevel; }

private:
    UPROPERTY()
    FOmnisNexusProgression Progression;
};
