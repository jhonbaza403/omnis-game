#include "Camera/Player/OmnisPlayerCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UOmnisPlayerCameraComponent::UOmnisPlayerCameraComponent()
{
    FieldOfView = 75.0f;
}

void UOmnisPlayerCameraComponent::SetDistance(float Distance)
{
    if (USpringArmComponent* Arm = Cast<USpringArmComponent>(GetAttachParent()))
    {
        Arm->TargetArmLength = FMath::Clamp(Distance, 200.0f, 1200.0f);
    }
}
