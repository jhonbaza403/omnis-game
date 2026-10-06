#include "World/Physics/OmnisPhysicsComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UOmnisPhysicsComponent::UOmnisPhysicsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UOmnisPhysicsComponent::SetSimulationEnabled(bool bEnabled)
{
    if (AActor* Owner = GetOwner())
    {
        if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
        {
            Primitive->SetSimulatePhysics(bEnabled);
        }
    }
}

void UOmnisPhysicsComponent::AddImpulse(FVector Impulse)
{
    if (AActor* Owner = GetOwner())
    {
        if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
        {
            Primitive->AddImpulse(Impulse, NAME_None, true);
        }
    }
}
