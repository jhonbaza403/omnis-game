#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisCheatSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisCheatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Developer")
    void SetCheatsEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category="Omnis|Developer")
    bool AreCheatsEnabled() const { return bCheatsEnabled; }

private:
    UPROPERTY()
    bool bCheatsEnabled = false;
};
