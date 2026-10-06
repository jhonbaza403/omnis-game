#include "Gameplay/Exploration/OmnisExplorationComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UOmnisExplorationComponent::UOmnisExplorationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

int32 UOmnisExplorationComponent::Scan()
{
    UWorld* World = GetWorld();
    const AActor* Owner = GetOwner();
    if (!World || !Owner)
    {
        return 0;
    }

    TArray<AActor*> Candidates;
    TArray<FHitResult> Hits;
    FCollisionShape Shape = FCollisionShape::MakeSphere(ScanRadius);

    World->SweepMultiByChannel(
        Hits,
        Owner->GetActorLocation(),
        Owner->GetActorLocation(),
        FQuat::Identity,
        ECC_Visibility,
        Shape
    );

    int32 NewDiscoveries = 0;
    for (const FHitResult& Hit : Hits)
    {
        const AActor* Actor = Hit.GetActor();
        if (!Actor || !Actor->ActorHasTag(TEXT("Omnis.Discoverable")))
        {
            continue;
        }

        const FName Identifier = Actor->GetFName();
        if (!Discoveries.Contains(Identifier))
        {
            Discoveries.Add(Identifier);
            ++NewDiscoveries;
        }
    }

    return NewDiscoveries;
}
