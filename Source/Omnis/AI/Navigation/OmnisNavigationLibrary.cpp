#include "AI/Navigation/OmnisNavigationLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

bool UOmnisNavigationLibrary::FindReachableLocation(UObject* WorldContextObject, FVector Origin, FVector Direction, float Distance, FVector& OutLocation)
{
    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    if (!World || Distance <= 0.0f || Direction.IsNearlyZero())
    {
        return false;
    }

    Direction.Normalize();
    const FVector Desired = Origin + Direction * Distance;
    FHitResult Hit;
    if (World->LineTraceSingleByChannel(Hit, Origin + FVector::UpVector * 50.0f, Desired + FVector::UpVector * 50.0f, ECC_Visibility))
    {
        return false;
    }

    OutLocation = Desired;
    return true;
}
