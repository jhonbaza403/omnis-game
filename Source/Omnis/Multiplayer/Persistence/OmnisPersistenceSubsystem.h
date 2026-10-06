#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisPersistenceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisPersistenceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    void SetPersistentInt(FName Key, int32 Value);

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    int32 GetPersistentInt(FName Key, int32 DefaultValue = 0) const;

private:
    UPROPERTY()
    TMap<FName, int32> PersistentInts;
};
