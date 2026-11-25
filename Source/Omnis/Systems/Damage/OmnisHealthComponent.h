#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmnisHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOmnisHealthChanged, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOmnisDeath);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisHealthComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Health")
    void Restore(float Amount);

    UFUNCTION(BlueprintCallable, Category="Omnis|Health")
    void SetMaxHealth(float NewMaxHealth);

    UFUNCTION(BlueprintCallable, Category="Omnis|Health")
    void SetDeadState(bool bNewDead);

    UFUNCTION(BlueprintCallable, Category="Omnis|Health")
    void RestoreSavedState(float SavedHealth, float SavedMaxHealth, bool bSavedDead);

    UFUNCTION(BlueprintPure, Category="Omnis|Health")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category="Omnis|Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category="Omnis|Health")
    bool IsDead() const { return bDead; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Omnis|Health")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Health")
    bool bDestroyOwnerOnDeath = true;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Health")
    FOnOmnisHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Health")
    FOnOmnisDeath OnDeath;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnRep_Health(float PreviousHealth);

    UFUNCTION()
    void OnRep_Dead();

    UFUNCTION()
    void HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

    UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Health)
    float Health = 100.0f;

    UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Dead)
    bool bDead = false;
};
