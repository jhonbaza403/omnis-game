#include "World/Wildlife/OmnisWildlifeActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOmnisWildlifeActor::AOmnisWildlifeActor()
{
    PrimaryActorTick.bCanEverTick = true;
    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    RootComponent = Visual;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded())
    {
        Visual->SetStaticMesh(Cube.Object);
    }

    Visual->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetActorScale3D(FVector(0.5f));
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
    SetActorLocation(GetActorLocation() + Offset * DeltaSeconds);
}
