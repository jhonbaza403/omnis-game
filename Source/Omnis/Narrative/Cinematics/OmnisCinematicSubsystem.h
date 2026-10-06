#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisCinematicSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisCinematicSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Cinematics")
    void PlaySequence(FName SequenceId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Cinematics")
    void StopSequence();

    UFUNCTION(BlueprintPure, Category="Omnis|Cinematics")
    FName GetActiveSequence() const { return ActiveSequence; }

private:
    UPROPERTY()
    FName ActiveSequence;
};
