#include "Gameplay/Inventory/OmnisInventoryComponent.h"

#include "GameFramework/Actor.h"

UOmnisInventoryComponent::UOmnisInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UOmnisInventoryComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisInventoryComponent, ReplicatedItems);
}

bool UOmnisInventoryComponent::HasMutationAuthority() const
{
    const AActor* Owner = GetOwner();
    return !Owner || Owner->HasAuthority();
}

bool UOmnisInventoryComponent::AddItem(FName ItemId, int32 Quantity)
{
    if (!HasMutationAuthority() || ItemId.IsNone() || Quantity <= 0)
    {
        return false;
    }

    const int32 SafeMaxStack = FMath::Clamp(MaxStackSize, 1, 9999);
    const int32 CurrentQuantity = Items.FindRef(ItemId);
    if (Quantity > SafeMaxStack || CurrentQuantity > SafeMaxStack - Quantity)
    {
        return false;
    }

    if (!Items.Contains(ItemId) &&
        Items.Num() >= FMath::Clamp(MaxUniqueItems, 1, 500))
    {
        return false;
    }

    Items.Add(ItemId, CurrentQuantity + Quantity);
    SyncReplicatedItems();
    OnInventoryChanged.Broadcast();
    return true;
}

bool UOmnisInventoryComponent::RemoveItem(FName ItemId, int32 Quantity)
{
    if (!HasMutationAuthority())
    {
        if (GetOwner())
        {
            ServerRemoveItem(ItemId, Quantity);
            return true;
        }
        return false;
    }

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

    SyncReplicatedItems();
    OnInventoryChanged.Broadcast();
    return true;
}

void UOmnisInventoryComponent::ServerRemoveItem_Implementation(
    FName ItemId,
    int32 Quantity)
{
    RemoveItem(ItemId, Quantity);
}

int32 UOmnisInventoryComponent::GetQuantity(FName ItemId) const
{
    return Items.FindRef(ItemId);
}

TArray<FOmnisInventoryEntry> UOmnisInventoryComponent::ExportItems() const
{
    TArray<FOmnisInventoryEntry> Result;
    Result.Reserve(Items.Num());

    for (const TPair<FName, int32>& Pair : Items)
    {
        if (Pair.Key.IsNone() || Pair.Value <= 0)
        {
            continue;
        }

        FOmnisInventoryEntry Entry;
        Entry.ItemId = Pair.Key;
        Entry.Quantity = Pair.Value;
        Result.Add(Entry);
    }

    Result.Sort([](const FOmnisInventoryEntry& A, const FOmnisInventoryEntry& B)
    {
        return A.ItemId.Compare(B.ItemId) < 0;
    });
    return Result;
}

void UOmnisInventoryComponent::ImportItems(
    const TArray<FOmnisInventoryEntry>& SavedItems)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    Items.Reset();
    for (const FOmnisInventoryEntry& Entry : SavedItems)
    {
        if (Entry.ItemId.IsNone() || Entry.Quantity <= 0)
        {
            continue;
        }

        const int32 Existing = Items.FindRef(Entry.ItemId);
        const int32 NewQuantity = FMath::Min(
            FMath::Clamp(MaxStackSize, 1, 9999),
            Existing + FMath::Min(Entry.Quantity, FMath::Clamp(MaxStackSize, 1, 9999)));

        if (Items.Contains(Entry.ItemId) ||
            Items.Num() < FMath::Clamp(MaxUniqueItems, 1, 500))
        {
            Items.Add(Entry.ItemId, NewQuantity);
        }
    }

    SyncReplicatedItems();
    OnInventoryChanged.Broadcast();
}

void UOmnisInventoryComponent::SyncReplicatedItems()
{
    ReplicatedItems = ExportItems();
    if (AActor* Owner = GetOwner())
    {
        Owner->ForceNetUpdate();
    }
}

void UOmnisInventoryComponent::OnRep_InventoryEntries()
{
    Items.Reset();
    for (const FOmnisInventoryEntry& Entry : ReplicatedItems)
    {
        if (!Entry.ItemId.IsNone() && Entry.Quantity > 0)
        {
            Items.Add(
                Entry.ItemId,
                FMath::Clamp(Entry.Quantity, 1, FMath::Clamp(MaxStackSize, 1, 9999)));
        }
    }

    OnInventoryChanged.Broadcast();
}
