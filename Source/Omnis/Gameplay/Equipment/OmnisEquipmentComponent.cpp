#include "Gameplay/Equipment/OmnisEquipmentComponent.h"

UOmnisEquipmentComponent::UOmnisEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UOmnisEquipmentComponent::Equip(FName Slot, FName ItemId)
{
    if (Slot.IsNone() || ItemId.IsNone())
    {
        return false;
    }

    Equipment.Add(Slot, ItemId);
    return true;
}

void UOmnisEquipmentComponent::Unequip(FName Slot)
{
    Equipment.Remove(Slot);
}

FName UOmnisEquipmentComponent::GetEquipped(FName Slot) const
{
    const FName* ItemId = Equipment.Find(Slot);
    return ItemId ? *ItemId : NAME_None;
}
