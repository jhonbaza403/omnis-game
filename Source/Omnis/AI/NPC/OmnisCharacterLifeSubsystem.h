#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisCharacterLifeSubsystem.generated.h"

class AOmnisNPCCharacter;

struct FOmnisRegisteredNPCEntry
{
    TWeakObjectPtr<AOmnisNPCCharacter> NPC;
    double LastSimulatedGameSeconds = 0.0;
};

UCLASS()
class OMNIS_API UOmnisCharacterLifeSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaSeconds) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UOmnisCharacterLifeSubsystem, STATGROUP_Tickables);
    }

    bool RegisterLocation(FName LocationId, const FVector& WorldLocation);
    FVector ResolveLocation(FName LocationId, const FVector& Fallback) const;

    void RegisterNPC(AOmnisNPCCharacter* NPC);
    void UnregisterNPC(AOmnisNPCCharacter* NPC);
    void RebaseRegisteredNPCsToCurrentTime();

    int32 GetRegisteredNPCCount() const { return RegisteredNPCs.Num(); }

private:
    static constexpr int32 MaxNPCUpdatesPerTick = 128;
    static constexpr float NavigationSimulationRadius = 6000.0f;

    TMap<FName, FVector> NamedLocations;
    TArray<FOmnisRegisteredNPCEntry> RegisteredNPCs;
    int32 NextNPCIndex = 0;
};
