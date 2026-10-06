#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisEquipmentComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisEquipmentComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Equipment")
    bool Equip(FName Slot, FName ItemId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Equipment")
    void Unequip(FName Slot);

    UFUNCTION(BlueprintPure, Category="Omnis|Equipment")
    FName GetEquipped(FName Slot) const;

private:
    UPROPERTY()
    TMap<FName, FName> Equipment;
};
