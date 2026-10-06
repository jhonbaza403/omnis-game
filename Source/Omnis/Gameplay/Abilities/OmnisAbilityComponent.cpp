#include "Gameplay/Abilities/OmnisAbilityComponent.h"
#include "Engine/World.h"

UOmnisAbilityComponent::UOmnisAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

bool UOmnisAbilityComponent::ActivateAbility(FName AbilityId)
{
    if (AbilityId.IsNone() || IsOnCooldown(AbilityId))
    {
        return false;
    }

    SetCooldown(AbilityId, 1.0f);
    return true;
}

void UOmnisAbilityComponent::SetCooldown(FName AbilityId, float Duration)
{
    if (AbilityId.IsNone() || Duration <= 0.0f)
    {
        return;
    }

    Cooldowns.Add(AbilityId, GetWorld() ? GetWorld()->GetTimeSeconds() + Duration : Duration);
}

bool UOmnisAbilityComponent::IsOnCooldown(FName AbilityId) const
{
    const float* Expiry = Cooldowns.Find(AbilityId);
    if (!Expiry || !GetWorld())
    {
        return false;
    }
    return GetWorld()->GetTimeSeconds() < *Expiry;
}
