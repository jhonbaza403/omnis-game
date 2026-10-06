#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisDebugSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisDebugSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Developer")
    void SetDebugEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category="Omnis|Developer")
    bool IsDebugEnabled() const { return bDebugEnabled; }

private:
    UPROPERTY()
    bool bDebugEnabled = false;
};
