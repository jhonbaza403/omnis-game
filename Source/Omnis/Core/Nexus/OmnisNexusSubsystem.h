#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisNexusSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisNexusProgression
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Nexus")
    int32 PermanentUpgradePoints = 0;

    UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category="Nexus")
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

    UFUNCTION(BlueprintPure, Category="Omnis|Nexus")
    int32 GetPermanentUpgradePoints() const { return Progression.PermanentUpgradePoints; }

    UFUNCTION(BlueprintPure, Category="Omnis|Nexus")
    int32 GetPermanentVitalityLevel() const { return Progression.PermanentVitalityLevel; }

    UFUNCTION(BlueprintPure, Category="Omnis|Nexus")
    FOmnisNexusProgression ExportProgression() const { return Progression; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Nexus")
    void ImportProgression(const FOmnisNexusProgression& SavedProgression);

private:
    UPROPERTY()
    FOmnisNexusProgression Progression;
};
