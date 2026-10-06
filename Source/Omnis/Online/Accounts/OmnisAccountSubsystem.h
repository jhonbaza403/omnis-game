#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAccountSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisAccountSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetDisplayName(FString NewName);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetDisplayName() const { return DisplayName; }

private:
    UPROPERTY()
    FString DisplayName;
};
