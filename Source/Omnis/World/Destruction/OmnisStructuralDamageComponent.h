#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmnisStructuralDamageComponent.generated.h"

USTRUCT(BlueprintType)
struct FOmnisStructuralPartDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Omnis|Destruction")
    FName PartId;

    // Tag assigned to a pre-authored UPrimitiveComponent on the owning building.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Omnis|Destruction")
    FName ComponentTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Omnis|Destruction", meta=(ClampMin="1.0"))
    float MaxDurability = 100.0f;
};

USTRUCT(BlueprintType)
struct FOmnisStructuralPartState
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    FName PartId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    FName ComponentTag;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    float MaxDurability = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    float CurrentDurability = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    bool bDestroyed = false;
};

USTRUCT(BlueprintType)
struct FOmnisStructuralDamageSnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    FName PersistentStructureId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Destruction")
    TArray<FOmnisStructuralPartState> Parts;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnOmnisStructuralDamageChanged,
    FName,
    PersistentStructureId);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisStructuralDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisStructuralDamageComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Destruction")
    bool ApplyStructuralDamage(FName PartId, float Damage);

    UFUNCTION(BlueprintCallable, Category="Omnis|Destruction")
    bool RepairStructuralPart(FName PartId, float RepairAmount);

    UFUNCTION(BlueprintPure, Category="Omnis|Destruction")
    float GetPartDurability(FName PartId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Destruction")
    bool IsPartDestroyed(FName PartId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Destruction")
    FOmnisStructuralDamageSnapshot ExportSnapshot() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Destruction")
    void ImportSnapshot(const FOmnisStructuralDamageSnapshot& Snapshot);

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Omnis|Destruction")
    FName PersistentStructureId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Omnis|Destruction")
    TArray<FOmnisStructuralPartDefinition> PartDefinitions;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Destruction")
    FOnOmnisStructuralDamageChanged OnDamageChanged;

private:
    UFUNCTION()
    void OnRep_PartStates();

    void InitializePartStates();
    void ApplyVisualState(const FOmnisStructuralPartState& PartState);
    void ApplyAllVisualStates();
    bool HasMutationAuthority() const;
    void NotifyStateChanged();

    UPROPERTY(SaveGame, ReplicatedUsing=OnRep_PartStates)
    TArray<FOmnisStructuralPartState> PartStates;
};
