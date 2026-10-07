#include "World/Wildlife/OmnisWildlifeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AOmnisWildlifeActor::AOmnisWildlifeActor()
{
    PrimaryActorTick.bCanEverTick = true;
    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    RootComponent = Visual;
    Visual->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AOmnisWildlifeActor::ConfigureWildlifeAsset(const FString& MeshAssetPath)
{
    WildlifeMeshPath = MeshAssetPath;
    ApplyConfiguredMesh();
}

void AOmnisWildlifeActor::ApplyConfiguredMesh()
{
    if (WildlifeMeshPath.IsEmpty())
    {
        return;
    }

    if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *WildlifeMeshPath))
    {
        Visual->SetStaticMesh(Mesh);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Omnis wildlife asset missing: %s"), *WildlifeMeshPath);
    }
}

void AOmnisWildlifeActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    WanderTime += DeltaSeconds;

    const FVector Offset(
        FMath::Sin(WanderTime * 0.7f) * 20.0f,
        FMath::Cos(WanderTime * 0.5f) * 16.0f,
        0.0f
    );

    AddActorWorldOffset(Offset * DeltaSeconds);
}
