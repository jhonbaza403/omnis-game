#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisTimeSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisTimeSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaSeconds) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UOmnisTimeSubsystem, STATGROUP_Tickables);
    }

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
