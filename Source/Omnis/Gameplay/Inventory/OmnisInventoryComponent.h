#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisInventoryComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisInventoryComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Inventory")
    bool AddItem(FName ItemId, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Inventory")
    bool RemoveItem(FName ItemId, int32 Quantity = 1);

    UFUNCTION(BlueprintPure, Category="Omnis|Inventory")
    int32 GetQuantity(FName ItemId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Inventory")
    const TMap<FName, int32>& GetItems() const { return Items; }

private:
    UPROPERTY()
    TMap<FName, int32> Items;
};
