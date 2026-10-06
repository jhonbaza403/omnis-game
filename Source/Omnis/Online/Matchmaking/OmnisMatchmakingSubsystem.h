#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisMatchmakingSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisMatchmakingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void Enqueue(FName PlaylistId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void Dequeue();

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsQueued() const { return bQueued; }

private:
    UPROPERTY()
    FName PlaylistId;

    UPROPERTY()
    bool bQueued = false;
};
