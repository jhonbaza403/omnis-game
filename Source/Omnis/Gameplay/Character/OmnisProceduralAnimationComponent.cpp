#include "Gameplay/Character/OmnisProceduralAnimationComponent.h"

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

UOmnisProceduralAnimationComponent::UOmnisProceduralAnimationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UOmnisProceduralAnimationComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        if (USceneComponent* Root = Owner->GetRootComponent())
        {
            BaseRelativeLocation = Root->GetRelativeLocation();
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

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    if (USceneComponent* Root = Owner->GetRootComponent())
    {
        const float Wave = FMath::Sin(TimeSeconds * BobFrequency * 2.0f * UE_TWO_PI);
        FVector Location = BaseRelativeLocation;
        Location.Z += Wave * BobAmplitude;
        Root->SetRelativeLocation(Location);

        FRotator Rotation = Root->GetRelativeRotation();
        Rotation.Yaw += FMath::Sin(TimeSeconds * BobFrequency * UE_TWO_PI) * TurnAmplitude * DeltaTime;
        Root->SetRelativeRotation(Rotation);
    }
}
