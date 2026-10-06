#include "Gameplay/Construction/OmnisConstructionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"

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
    AActor* Piece = World->SpawnActor<AActor>(AActor::StaticClass(), Location, Owner->GetActorRotation());
    if (!Piece)
    {
        return nullptr;
    }

    UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Piece, TEXT("ConstructionMesh"));
    Piece->SetRootComponent(Mesh);
    Mesh->RegisterComponent();

    if (UStaticMesh* CubeAsset = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        Mesh->SetStaticMesh(CubeAsset);
    }

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetSimulatePhysics(false);
    Piece->SetActorScale3D(FVector(1.2f, 1.2f, 0.25f));
    Piece->Tags.Add(TEXT("Omnis.Construction"));

    return Piece;
}
