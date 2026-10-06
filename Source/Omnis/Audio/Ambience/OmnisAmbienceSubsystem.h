#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisAmbienceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisAmbienceSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Audio")
    void SetAmbience(FName AmbienceId);

    UFUNCTION(BlueprintPure, Category="Omnis|Audio")
    FName GetAmbience() const { return AmbienceId; }

private:
    UPROPERTY()
    FName AmbienceId;
};
