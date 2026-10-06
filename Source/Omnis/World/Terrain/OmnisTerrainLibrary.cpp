#include "World/Terrain/OmnisTerrainLibrary.h"
#include "Engine/World.h"

bool UOmnisTerrainLibrary::FindGround(UObject* WorldContextObject, FVector Origin, float TraceHeight, FVector& OutLocation)
{
    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    if (!World || TraceHeight <= 0.0f)
    {
        return false;
    }

    const FVector Start = Origin + FVector::UpVector * TraceHeight;
    const FVector End = Origin - FVector::UpVector * TraceHeight;
    FHitResult Hit;

    if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        return false;
    }

    OutLocation = Hit.ImpactPoint;
    return true;
}
