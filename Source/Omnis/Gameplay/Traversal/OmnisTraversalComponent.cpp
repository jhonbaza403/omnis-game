#include "Gameplay/Traversal/OmnisTraversalComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UOmnisTraversalComponent::UOmnisTraversalComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UOmnisTraversalComponent::TryVault()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return false;
    }

    const FVector Launch = Character->GetActorForwardVector() * VaultLaunchStrength + FVector::UpVector * (VaultLaunchStrength * 0.55f);
    Character->LaunchCharacter(Launch, false, true);
    return true;
}
