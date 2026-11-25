#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisEconomySimulationSubsystem.generated.h"

/** Advances the autonomous regional production/consumption model by game day. */
UCLASS()
class OMNIS_API UOmnisEconomySimulationSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Tick(float DeltaSeconds) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UOmnisEconomySimulationSubsystem, STATGROUP_Tickables);
    }
private:
    static constexpr double SecondsPerGameDay = 86400.0;
    int64 LastSimulatedGameDay = INDEX_NONE;
};
