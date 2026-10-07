#include "Gameplay/Construction/OmnisConstructionComponent.h"

#include "CollisionQueryParams.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"

UOmnisConstructionComponent::UOmnisConstructionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FVector UOmnisConstructionComponent::SnapLocation(const FVector& Location) const
{
    const float SafeGrid = FMath::Max(1.0f, GridSize);
    return FVector(
        FMath::GridSnap(Location.X, SafeGrid),
        FMath::GridSnap(Location.Y, SafeGrid),
        FMath::GridSnap(Location.Z, SafeGrid));
}

void UOmnisConstructionComponent::ServerConstructForward_Implementation()
{
    ConstructForward();
}

AActor* UOmnisConstructionComponent::ConstructForward()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();

    if (!Owner || !World || !Owner->HasAuthority() || BuiltPieces.Num() >= MaxPieces)
    {
        return nullptr;
    }

    const FVector RawLocation =
        Owner->GetActorLocation() + Owner->GetActorForwardVector() * BuildDistance;
    const FVector Location = SnapLocation(RawLocation);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(OmnisBuild), false, Owner);
    const FVector TraceStart = Owner->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
    const FVector TraceEnd = Location + FVector(0.0f, 0.0f, 50.0f);

    if (World->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_Visibility, Params) && Hit.GetActor() != nullptr)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = Owner;
    SpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Piece = World->SpawnActor<AActor>(
        AActor::StaticClass(),
        Location,
        FRotator(0.0f, Owner->GetActorRotation().Yaw, 0.0f),
        SpawnParameters);

    if (!Piece)
    {
        return nullptr;
    }

    UStaticMeshComponent* Mesh =
        NewObject<UStaticMeshComponent>(Piece, TEXT("ConstructionMesh"));

    Piece->SetRootComponent(Mesh);
    Mesh->RegisterComponent();

    if (UStaticMesh* CubeAsset =
        LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        Mesh->SetStaticMesh(CubeAsset);
    }

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetMobility(EComponentMobility::Movable);
    Piece->SetActorScale3D(FVector(1.0f, 1.0f, 0.25f));

    if (UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(
        nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
    {
        if (UMaterialInstanceDynamic* Material =
            UMaterialInstanceDynamic::Create(BaseMaterial, Mesh))
        {
            Material->SetVectorParameterValue(
                TEXT("BaseColor"), FLinearColor(0.08f, 0.55f, 0.95f, 1.0f));
            Material->SetScalarParameterValue(TEXT("Roughness"), 0.34f);
            Mesh->SetMaterial(0, Material);
        }
    }

    Piece->SetReplicates(true);
    Piece->SetReplicateMovement(true);
    Piece->Tags.Add(TEXT("Omnis.Construction"));
    BuiltPieces.Add(Piece);
    return Piece;
}

bool UOmnisConstructionComponent::RemoveLastPiece()
{
    while (BuiltPieces.Num() > 0)
    {
        TObjectPtr<AActor> Piece = BuiltPieces.Pop();
        if (IsValid(Piece))
        {
            Piece->Destroy();
            return true;
        }
    }

    return false;
}

int32 UOmnisConstructionComponent::GetBuiltPieceCount() const
{
    int32 Count = 0;
    for (const TObjectPtr<AActor>& Piece : BuiltPieces)
    {
        Count += IsValid(Piece) ? 1 : 0;
    }

    return Count;
}
