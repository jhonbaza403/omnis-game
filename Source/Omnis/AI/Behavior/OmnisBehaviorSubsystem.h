#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisBehaviorSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisBehaviorSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|AI")
    void SetBehaviorForActor(AActor* Actor, FName Behavior);

    UFUNCTION(BlueprintPure, Category="Omnis|AI")
    FName GetBehaviorForActor(AActor* Actor) const;

private:
    UPROPERTY()
    TMap<FName, FName> Behaviors;
};
