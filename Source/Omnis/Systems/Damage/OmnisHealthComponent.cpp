#include "Systems/Damage/OmnisHealthComponent.h"
#include "GameFramework/Actor.h"

UOmnisHealthComponent::UOmnisHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UOmnisHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    Health = FMath::Max(1.0f, MaxHealth);

    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UOmnisHealthComponent::HandleDamage);
    }
}

void UOmnisHealthComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (bDead || Damage <= 0.0f)
    {
        return;
    }

    const float Previous = Health;
    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, Health - Previous);

    if (Health <= 0.0f)
    {
        bDead = true;
        OnDeath.Broadcast();
        DamagedActor->SetLifeSpan(0.2f);
    }
}

void UOmnisHealthComponent::Restore(float Amount)
{
    if (bDead || Amount <= 0.0f)
    {
        return;
    }

    const float Previous = Health;
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, Health - Previous);
}

void UOmnisHealthComponent::SetMaxHealth(float NewMaxHealth)
{
    MaxHealth = FMath::Max(1.0f, NewMaxHealth);
    Health = FMath::Clamp(Health, 0.0f, MaxHealth);
}
