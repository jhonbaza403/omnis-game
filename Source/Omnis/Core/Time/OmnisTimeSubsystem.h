#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisTimeSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisTimeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UOmnisTimeSubsystem();

    virtual void Tick(float DeltaSeconds);
    virtual bool IsTickable() const { return true; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Time")
    void SetTimeScale(float NewScale);

    UFUNCTION(BlueprintPure, Category="Omnis|Time")
    float GetElapsedSeconds() const { return ElapsedSeconds; }

    UFUNCTION(BlueprintPure, Category="Omnis|Time")
    float GetTimeScale() const { return TimeScale; }

private:
    UPROPERTY()
    float ElapsedSeconds = 0.0f;

    UPROPERTY()
    float TimeScale = 1.0f;
};
