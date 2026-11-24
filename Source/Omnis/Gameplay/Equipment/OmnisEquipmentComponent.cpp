#include "Gameplay/Equipment/OmnisEquipmentComponent.h"

#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "GameFramework/Actor.h"

UOmnisEquipmentComponent::UOmnisEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UOmnisEquipmentComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisEquipmentComponent, ReplicatedEquipment);
}

bool UOmnisEquipmentComponent::HasMutationAuthority() const
{
    const AActor* Owner = GetOwner();
    return !Owner || Owner->HasAuthority();
}

bool UOmnisEquipmentComponent::Equip(FName Slot, FName ItemId)
{
    if (Slot.IsNone() || ItemId.IsNone())
    {
        return false;
    }

    if (!HasMutationAuthority())
    {
        ServerEquip(Slot, ItemId);
        return true;
    }

    return EquipOnAuthority(Slot, ItemId);
}

void UOmnisEquipmentComponent::ServerEquip_Implementation(FName Slot, FName ItemId)
{
    EquipOnAuthority(Slot, ItemId);
}

bool UOmnisEquipmentComponent::EquipOnAuthority(FName Slot, FName ItemId)
{
    if (!HasMutationAuthority() || Slot.IsNone() || ItemId.IsNone())
    {
        return false;
    }

    AOmnisPlayerCharacter* Player = Cast<AOmnisPlayerCharacter>(GetOwner());
    if (!Player || !Player->Inventory)
    {
        return false;
    }

    const FName PreviousItem = Equipment.FindRef(Slot);
    if (PreviousItem == ItemId)
    {
        return true;
    }

    if (Player->Inventory->GetQuantity(ItemId) < 1 ||
        !Player->Inventory->RemoveItem(ItemId, 1))
    {
        return false;
    }

    if (!PreviousItem.IsNone() && !Player->Inventory->AddItem(PreviousItem, 1))
    {
        // Roll back the requested item if the displaced item cannot be returned.
        Player->Inventory->AddItem(ItemId, 1);
        return false;
    }

    Equipment.Add(Slot, ItemId);
    SyncReplicatedEquipment();
    OnEquipmentChanged.Broadcast();
    return true;
}

void UOmnisEquipmentComponent::Unequip(FName Slot)
{
    if (Slot.IsNone())
    {
        return;
    }

    if (!HasMutationAuthority())
    {
        ServerUnequip(Slot);
        return;
    }

    UnequipOnAuthority(Slot);
}

void UOmnisEquipmentComponent::ServerUnequip_Implementation(FName Slot)
{
    UnequipOnAuthority(Slot);
}

void UOmnisEquipmentComponent::UnequipOnAuthority(FName Slot)
{
    if (!HasMutationAuthority() || Slot.IsNone())
    {
        return;
    }

    AOmnisPlayerCharacter* Player = Cast<AOmnisPlayerCharacter>(GetOwner());
    if (!Player || !Player->Inventory)
    {
        return;
    }

    const FName ExistingItem = Equipment.FindRef(Slot);
    if (ExistingItem.IsNone() || !Player->Inventory->AddItem(ExistingItem, 1))
    {
        return;
    }

    Equipment.Remove(Slot);
    SyncReplicatedEquipment();
    OnEquipmentChanged.Broadcast();
}

FName UOmnisEquipmentComponent::GetEquipped(FName Slot) const
{
    return Equipment.FindRef(Slot);
}

TArray<FOmnisEquipmentEntry> UOmnisEquipmentComponent::ExportEquipment() const
{
    TArray<FOmnisEquipmentEntry> Result;
    Result.Reserve(Equipment.Num());

    for (const TPair<FName, FName>& Pair : Equipment)
    {
        if (Pair.Key.IsNone() || Pair.Value.IsNone())
        {
            continue;
        }

        FOmnisEquipmentEntry Entry;
        Entry.Slot = Pair.Key;
        Entry.ItemId = Pair.Value;
        Result.Add(Entry);
    }

    Result.Sort([](const FOmnisEquipmentEntry& A, const FOmnisEquipmentEntry& B)
    {
        return A.Slot.Compare(B.Slot) < 0;
    });
    return Result;
}

void UOmnisEquipmentComponent::ImportEquipment(
    const TArray<FOmnisEquipmentEntry>& SavedEquipment)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    Equipment.Reset();
    for (const FOmnisEquipmentEntry& Entry : SavedEquipment)
    {
        if (!Entry.Slot.IsNone() && !Entry.ItemId.IsNone())
        {
            Equipment.Add(Entry.Slot, Entry.ItemId);
        }
    }

    SyncReplicatedEquipment();
    OnEquipmentChanged.Broadcast();
}

void UOmnisEquipmentComponent::SyncReplicatedEquipment()
{
    ReplicatedEquipment = ExportEquipment();
    if (AActor* Owner = GetOwner())
    {
        Owner->ForceNetUpdate();
    }
}

void UOmnisEquipmentComponent::OnRep_EquipmentEntries()
{
    Equipment.Reset();
    for (const FOmnisEquipmentEntry& Entry : ReplicatedEquipment)
    {
        if (!Entry.Slot.IsNone() && !Entry.ItemId.IsNone())
        {
            Equipment.Add(Entry.Slot, Entry.ItemId);
        }
    }

    OnEquipmentChanged.Broadcast();
}
