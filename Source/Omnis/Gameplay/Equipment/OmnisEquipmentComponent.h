#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmnisEquipmentComponent.generated.h"

USTRUCT(BlueprintType)
struct FOmnisEquipmentEntry
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Equipment")
    FName Slot;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Equipment")
    FName ItemId;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOmnisEquipmentChanged);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisEquipmentComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Equipment")
    bool Equip(FName Slot, FName ItemId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Equipment")
    void Unequip(FName Slot);

    UFUNCTION(BlueprintPure, Category="Omnis|Equipment")
    FName GetEquipped(FName Slot) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Equipment")
    TArray<FOmnisEquipmentEntry> ExportEquipment() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Equipment")
    void ImportEquipment(const TArray<FOmnisEquipmentEntry>& SavedEquipment);

    UPROPERTY(BlueprintAssignable, Category="Omnis|Equipment")
    FOnOmnisEquipmentChanged OnEquipmentChanged;

private:
    UFUNCTION(Server, Reliable)
    void ServerEquip(FName Slot, FName ItemId);

    UFUNCTION(Server, Reliable)
    void ServerUnequip(FName Slot);

    UFUNCTION()
    void OnRep_EquipmentEntries();

    bool EquipOnAuthority(FName Slot, FName ItemId);
    void UnequipOnAuthority(FName Slot);
    void SyncReplicatedEquipment();
    bool HasMutationAuthority() const;

    UPROPERTY(SaveGame)
    TMap<FName, FName> Equipment;

    UPROPERTY(ReplicatedUsing=OnRep_EquipmentEntries)
    TArray<FOmnisEquipmentEntry> ReplicatedEquipment;
};
