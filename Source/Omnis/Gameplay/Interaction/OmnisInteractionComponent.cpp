#include "Gameplay/Interaction/OmnisInteractionComponent.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UOmnisInteractionComponent::UOmnisInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UOmnisInteractionComponent::Interact()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World)
    {
        return false;
    }

    const FVector Start = Owner->GetActorLocation() + FVector::UpVector * 50.0f;
    const FVector End = Start + Owner->GetActorForwardVector() * InteractionDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(OmnisInteract), false, Owner);
    if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return false;
    }

    AActor* Target = Hit.GetActor();
    if (!Target || !Target->GetClass()->ImplementsInterface(UOmnisInteractableInterface::StaticClass()))
    {
        return false;
    }

    IOmnisInteractableInterface::Execute_Interact(Target, Owner);
    return true;
}
