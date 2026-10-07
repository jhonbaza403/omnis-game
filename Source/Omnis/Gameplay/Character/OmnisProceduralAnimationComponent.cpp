#include "Gameplay/Character/OmnisProceduralAnimationComponent.h"

#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

UOmnisProceduralAnimationComponent::UOmnisProceduralAnimationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UOmnisProceduralAnimationComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        AnimatedComponent = Owner->FindComponentByClass<UStaticMeshComponent>();
        if (AnimatedComponent)
        {
            BaseRelativeLocation = AnimatedComponent->GetRelativeLocation();
            BaseRelativeRotation = AnimatedComponent->GetRelativeRotation();
        }
    }
}

void UOmnisProceduralAnimationComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    TimeSeconds += DeltaTime;

    if (!AnimatedComponent)
    {
        return;
    }

    const float Wave = FMath::Sin(TimeSeconds * BobFrequency * 2.0f * PI);
    FVector Location = BaseRelativeLocation;
    Location.Z += Wave * BobAmplitude;
    AnimatedComponent->SetRelativeLocation(Location);

    FRotator Rotation = BaseRelativeRotation;
    Rotation.Yaw += FMath::Sin(TimeSeconds * BobFrequency * 2.0f * PI) * TurnAmplitude;
    AnimatedComponent->SetRelativeRotation(Rotation);
}
