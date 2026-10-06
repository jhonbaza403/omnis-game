#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisMultiplayerWorldSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisMultiplayerWorldSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    void SetWorldId(FName NewWorldId);

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    FName GetWorldId() const { return WorldId; }

private:
    UPROPERTY()
    FName WorldId;
};
