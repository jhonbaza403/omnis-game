#include "AI/Perception/OmnisPerceptionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UOmnisPerceptionComponent::UOmnisPerceptionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

AActor* UOmnisPerceptionComponent::SenseClosest(float Radius)
{
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !Owner || Radius <= 0.0f)
    {
        return nullptr;
    }

    TArray<FOverlapResult> Results;
    const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
    World->OverlapMultiByObjectType(
        Results,
        Owner->GetActorLocation(),
        FQuat::Identity,
        FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects),
        Shape
    );

    AActor* Best = nullptr;
    float BestDistanceSq = TNumericLimits<float>::Max();
    for (const FOverlapResult& Result : Results)
    {
        AActor* Candidate = Result.GetActor();
        if (!Candidate || Candidate == Owner)
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared(Owner->GetActorLocation(), Candidate->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            Best = Candidate;
            BestDistanceSq = DistanceSq;
        }
    }

    LastSensed = Best;
    return Best;
}
