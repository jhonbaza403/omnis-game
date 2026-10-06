#include "Systems/StatusEffects/OmnisStatusEffectComponent.h"
#include "Engine/World.h"

UOmnisStatusEffectComponent::UOmnisStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UOmnisStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetWorld())
    {
        return;
    }

    const float Now = GetWorld()->GetTimeSeconds();
    for (auto It = EffectExpiry.CreateIterator(); It; ++It)
    {
        if (Now >= It.Value())
        {
            It.RemoveCurrent();
        }
    }
}

void UOmnisStatusEffectComponent::ApplyEffect(FName EffectId, float Duration)
{
    if (!EffectId.IsNone() && Duration > 0.0f && GetWorld())
    {
        EffectExpiry.Add(EffectId, GetWorld()->GetTimeSeconds() + Duration);
    }
}

void UOmnisStatusEffectComponent::ClearEffect(FName EffectId)
{
    EffectExpiry.Remove(EffectId);
}

bool UOmnisStatusEffectComponent::HasEffect(FName EffectId) const
{
    const float* Expiry = EffectExpiry.Find(EffectId);
    return GetWorld() && Expiry && GetWorld()->GetTimeSeconds() < *Expiry;
}
