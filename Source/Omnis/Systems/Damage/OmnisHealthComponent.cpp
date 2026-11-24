#include "Systems/Damage/OmnisHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UOmnisHealthComponent::UOmnisHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UOmnisHealthComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisHealthComponent, MaxHealth);
    DOREPLIFETIME(UOmnisHealthComponent, Health);
    DOREPLIFETIME(UOmnisHealthComponent, bDead);
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

void UOmnisHealthComponent::HandleDamage(
    AActor* DamagedActor,
    float Damage,
    const UDamageType* DamageType,
    AController* InstigatedBy,
    AActor* DamageCauser)
{
    if ((DamagedActor && !DamagedActor->HasAuthority()) ||
        bDead || !FMath::IsFinite(Damage) || Damage <= 0.0f)
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
        if (bDestroyOwnerOnDeath && DamagedActor)
        {
            DamagedActor->SetLifeSpan(0.2f);
        }
    }
}

void UOmnisHealthComponent::Restore(float Amount)
{
    if ((GetOwner() && !GetOwner()->HasAuthority()) || bDead || !FMath::IsFinite(Amount) || Amount <= 0.0f)
    {
        return;
    }

    const float Previous = Health;
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, Health - Previous);
}

void UOmnisHealthComponent::SetMaxHealth(float NewMaxHealth)
{
    if (GetOwner() && !GetOwner()->HasAuthority())
    {
        return;
    }
    MaxHealth = FMath::Max(1.0f, FMath::IsFinite(NewMaxHealth) ? NewMaxHealth : 1.0f);
    Health = FMath::Clamp(Health, 0.0f, MaxHealth);
}

void UOmnisHealthComponent::SetDeadState(bool bNewDead)
{
    RestoreSavedState(Health, MaxHealth, bNewDead);
}

void UOmnisHealthComponent::RestoreSavedState(
    float SavedHealth,
    float SavedMaxHealth,
    bool bSavedDead)
{
    if (GetOwner() && !GetOwner()->HasAuthority())
    {
        return;
    }
    MaxHealth = FMath::Max(1.0f, FMath::IsFinite(SavedMaxHealth) ? SavedMaxHealth : 1.0f);
    bDead = bSavedDead;
    Health = bDead
        ? 0.0f
        : FMath::Clamp(FMath::IsFinite(SavedHealth) ? SavedHealth : MaxHealth, 0.0f, MaxHealth);
    if (!bDead && Health <= 0.0f)
    {
        Health = 1.0f;
    }
}
