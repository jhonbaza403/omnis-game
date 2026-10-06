#include "Gameplay/Construction/OmnisConstructionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

UOmnisConstructionComponent::UOmnisConstructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

AActor* UOmnisConstructionComponent::ConstructForward()
{
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !Owner)
    {
        return nullptr;
    }

    const FVector Location = Owner->GetActorLocation() + Owner->GetActorForwardVector() * BuildDistance;
    AActor* Piece = World->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator);
    if (!Piece)
    {
        return nullptr;
    }

    UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Piece, TEXT("ConstructionMesh"));
    Mesh->RegisterComponent();
    Piece->SetRootComponent(Mesh);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeAsset.Succeeded())
    {
        Mesh->SetStaticMesh(CubeAsset.Object);
    }

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetSimulatePhysics(false);
    Piece->SetActorScale3D(FVector(1.2f, 1.2f, 0.25f));
    Piece->Tags.Add(TEXT("Omnis.Construction"));

    return Piece;
}
