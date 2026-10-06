#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisProfilingSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisProfilingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Developer")
    void StartMarker(FName MarkerId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Developer")
    void EndMarker(FName MarkerId);

    UFUNCTION(BlueprintPure, Category="Omnis|Developer")
    float GetLastDuration(FName MarkerId) const;

private:
    TMap<FName, double> Started;
    UPROPERTY()
    TMap<FName, float> Durations;
};
