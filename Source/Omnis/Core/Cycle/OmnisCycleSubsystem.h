#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisCycleTypes.h"
#include "OmnisCycleSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnisCyclePhaseChanged, EOmnisCyclePhase, NewPhase);

UCLASS()
class OMNIS_API UOmnisCycleSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Cycle")
    void BeginCycle();

    UFUNCTION(BlueprintCallable, Category="Omnis|Cycle")
    void EnterNexus();

    UFUNCTION(BlueprintCallable, Category="Omnis|Cycle")
    void BeginRebirth();

    UFUNCTION(BlueprintCallable, Category="Omnis|Cycle")
    void AdvanceWorldAge(float DeltaYears);

    UFUNCTION(BlueprintPure, Category="Omnis|Cycle")
    FOmnisCycleState GetCycleState() const { return State; }

    UPROPERTY(BlueprintAssignable, Category="Omnis|Cycle")
    FOnOmnisCyclePhaseChanged OnPhaseChanged;

private:
    void SetPhase(EOmnisCyclePhase NewPhase);

    UPROPERTY()
    FOmnisCycleState State;
};
