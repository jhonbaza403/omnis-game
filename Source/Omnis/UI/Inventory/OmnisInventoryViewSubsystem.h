#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisInventoryViewSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisInventoryViewSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void FocusItem(FName ItemId);

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    FName GetFocusedItem() const { return FocusedItem; }

private:
    UPROPERTY()
    FName FocusedItem;
};
