#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisNexusViewSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisNexusViewSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetFocusedUpgrade(FName UpgradeId);

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    FName GetFocusedUpgrade() const { return FocusedUpgrade; }

private:
    UPROPERTY()
    FName FocusedUpgrade;
};
