#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisResourceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisResourceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Resources")
    void AddResource(FName ResourceId, int32 Amount);

    UFUNCTION(BlueprintCallable, Category="Omnis|Resources")
    bool ConsumeResource(FName ResourceId, int32 Amount);

    UFUNCTION(BlueprintPure, Category="Omnis|Resources")
    int32 GetResource(FName ResourceId) const;

private:
    UPROPERTY()
    TMap<FName, int32> Resources;
};
