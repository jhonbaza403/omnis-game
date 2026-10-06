#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisIdentitySubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisIdentitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Identity")
    void SetPlayerId(FString NewPlayerId);

    UFUNCTION(BlueprintPure, Category="Omnis|Identity")
    FString GetPlayerId() const { return PlayerId; }

private:
    UPROPERTY()
    FString PlayerId;
};
