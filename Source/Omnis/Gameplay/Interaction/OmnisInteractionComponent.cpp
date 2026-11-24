#include "Gameplay/Interaction/OmnisInteractionComponent.h"

#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UOmnisInteractionComponent::UOmnisInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

bool UOmnisInteractionComponent::Interact()
{
    AActor* Owner = GetOwner();
    if (!Owner || !GetWorld())
    {
        return false;
    }

    if (Owner->HasAuthority())
    {
        return PerformInteractionOnAuthority();
    }

    ServerInteract();
    return true;
}

void UOmnisInteractionComponent::ServerInteract_Implementation()
{
    PerformInteractionOnAuthority();
}

bool UOmnisInteractionComponent::PerformInteractionOnAuthority()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World || !Owner->HasAuthority())
    {
        return false;
    }

    const float SafeDistance = FMath::Clamp(
        FMath::IsFinite(InteractionDistance) ? InteractionDistance : 300.0f,
        50.0f,
        500.0f);

    const FVector Start = Owner->GetActorLocation() + FVector::UpVector * 50.0f;
    const FVector End = Start + Owner->GetActorForwardVector() * SafeDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(OmnisInteract), false, Owner);
    if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return false;
    }

    AActor* Target = Hit.GetActor();
    if (!Target || FVector::DistSquared(Owner->GetActorLocation(), Target->GetActorLocation()) >
        FMath::Square(SafeDistance) ||
        !Target->GetClass()->ImplementsInterface(UOmnisInteractableInterface::StaticClass()))
    {
        return false;
    }

    IOmnisInteractableInterface::Execute_Interact(Target, Owner);
    return true;
}
