#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmnisAbilityComponent.generated.h"

USTRUCT(BlueprintType)
struct FOmnisSavedAbilityCooldown
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Abilities")
    FName AbilityId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Abilities")
    float RemainingSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct FOmnisAbilityCooldownEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Abilities")
    FName AbilityId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Abilities")
    float EndServerTime = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnisAbilityActivated, FName, AbilityId);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisAbilityComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Abilities")
    bool ActivateAbility(FName AbilityId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Abilities")
    void SetCooldown(FName AbilityId, float Duration);

    UFUNCTION(BlueprintPure, Category="Omnis|Abilities")
    bool IsOnCooldown(FName AbilityId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Abilities")
    float GetRemainingCooldown(FName AbilityId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Abilities")
    TArray<FOmnisSavedAbilityCooldown> ExportCooldowns() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Abilities")
    void ImportCooldowns(const TArray<FOmnisSavedAbilityCooldown>& SavedCooldowns);

    UPROPERTY(BlueprintAssignable, Category="Omnis|Abilities")
    FOnOmnisAbilityActivated OnAbilityActivated;

private:
    UFUNCTION(Server, Reliable)
    void ServerActivateAbility(FName AbilityId);

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastAbilityActivated(FName AbilityId);

    UFUNCTION()
    void OnRep_CooldownEntries();

    bool ActivateAbilityOnAuthority(FName AbilityId);
    float GetCurrentServerTime() const;
    void PruneExpiredCooldowns();
    void SyncReplicatedCooldowns();

    UPROPERTY(SaveGame)
    TMap<FName, float> Cooldowns;

    UPROPERTY(ReplicatedUsing=OnRep_CooldownEntries)
    TArray<FOmnisAbilityCooldownEntry> ReplicatedCooldowns;
};
