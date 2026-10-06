#include "Gameplay/Inventory/OmnisInventoryComponent.h"

UOmnisInventoryComponent::UOmnisInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UOmnisInventoryComponent::AddItem(FName ItemId, int32 Quantity)
{
    if (ItemId.IsNone() || Quantity <= 0)
    {
        return false;
    }

    Items.FindOrAdd(ItemId) += Quantity;
    return true;
}

bool UOmnisInventoryComponent::RemoveItem(FName ItemId, int32 Quantity)
{
    const int32* Current = Items.Find(ItemId);
    if (ItemId.IsNone() || !Current || Quantity <= 0 || *Current < Quantity)
    {
        return false;
    }

    const int32 Remaining = *Current - Quantity;
    if (Remaining == 0)
    {
        Items.Remove(ItemId);
    }
    else
    {
        Items.Add(ItemId, Remaining);
    }
    return true;
}

int32 UOmnisInventoryComponent::GetQuantity(FName ItemId) const
{
    const int32* Current = Items.Find(ItemId);
    return Current ? *Current : 0;
}
