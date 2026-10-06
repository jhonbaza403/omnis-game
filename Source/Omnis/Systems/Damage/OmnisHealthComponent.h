#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOmnisHealthChanged, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOmnisDeath);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisHealthComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Health")
    void Restore(float Amount);

    UFUNCTION(BlueprintCallable, Category="Omnis|Health")
    void SetMaxHealth(float NewMaxHealth);

    UFUNCTION(BlueprintPure, Category="Omnis|Health")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category="Omnis|Health")
    float GetMaxHealth() const { return MaxHealth; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Health")
    float MaxHealth = 100.0f;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Health")
    FOnOmnisHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Health")
    FOnOmnisDeath OnDeath;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

    UPROPERTY()
    float Health = 100.0f;

    UPROPERTY()
    bool bDead = false;
};
