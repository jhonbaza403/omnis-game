#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisWorldStateSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisWorldStateSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|WorldState")
    void SetState(FName Key, int32 Value);

    UFUNCTION(BlueprintPure, Category="Omnis|WorldState")
    int32 GetState(FName Key, int32 DefaultValue = 0) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|WorldState")
    void IncrementState(FName Key, int32 Delta = 1);

private:
    UPROPERTY()
    TMap<FName, int32> Values;
};
