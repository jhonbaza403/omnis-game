#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisOnlineServiceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisOnlineServiceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetServiceAvailable(FName ServiceId, bool bAvailable);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsServiceAvailable(FName ServiceId) const;

private:
    UPROPERTY()
    TMap<FName, bool> Services;
};
