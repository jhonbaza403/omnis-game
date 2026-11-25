#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmnisInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FOmnisInventoryEntry
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Inventory")
    FName ItemId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Inventory")
    int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOmnisInventoryChanged);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisInventoryComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Adding inventory is an authority-only operation; pickups/shops must validate on the server.
    UFUNCTION(BlueprintCallable, Category="Omnis|Inventory")
    bool AddItem(FName ItemId, int32 Quantity = 1);

    // Removing inventory is safe to request from a client; the server revalidates the balance.
    UFUNCTION(BlueprintCallable, Category="Omnis|Inventory")
    bool RemoveItem(FName ItemId, int32 Quantity = 1);

    UFUNCTION(BlueprintPure, Category="Omnis|Inventory")
    int32 GetQuantity(FName ItemId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Inventory")
    const TMap<FName, int32>& GetItems() const { return Items; }

    UFUNCTION(BlueprintPure, Category="Omnis|Inventory")
    TArray<FOmnisInventoryEntry> ExportItems() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Inventory")
    void ImportItems(const TArray<FOmnisInventoryEntry>& SavedItems);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Inventory", meta=(ClampMin="1", ClampMax="500"))
    int32 MaxUniqueItems = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Inventory", meta=(ClampMin="1", ClampMax="9999"))
    int32 MaxStackSize = 999;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Inventory")
    FOnOmnisInventoryChanged OnInventoryChanged;

private:
    UFUNCTION(Server, Reliable)
    void ServerRemoveItem(FName ItemId, int32 Quantity);

    UFUNCTION()
    void OnRep_InventoryEntries();

    void SyncReplicatedItems();
    bool HasMutationAuthority() const;

    UPROPERTY(SaveGame)
    TMap<FName, int32> Items;

    UPROPERTY(ReplicatedUsing=OnRep_InventoryEntries)
    TArray<FOmnisInventoryEntry> ReplicatedItems;
};
