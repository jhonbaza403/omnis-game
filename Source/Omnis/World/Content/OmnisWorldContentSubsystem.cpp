#include "World/Content/OmnisWorldContentSubsystem.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Gameplay/Aviation/OmnisAircraftPawn.h"
#include "Gameplay/Vehicle/OmnisVehiclePawn.h"
#include "World/Wildlife/OmnisWildlifeActor.h"

namespace
{
    FString AssetPath(const TCHAR* Family, const TCHAR* Asset)
    {
        return FString::Printf(TEXT("/Game/Omnis/Art/%s/%s.%s"), Family, Asset, Asset);
    }
}

AActor* UOmnisWorldContentSubsystem::SpawnStaticAsset(const FString& AssetPathValue, const FVector& Location, const FVector& Scale)
{
    UWorld* World = GetWorld();
    UStaticMesh* Mesh = AssetPathValue.IsEmpty() ? nullptr : LoadObject<UStaticMesh>(nullptr, *AssetPathValue);

    if (!World || !Mesh)
    {
        UE_LOG(LogTemp, Error, TEXT("Omnis content asset unavailable: %s"), *AssetPathValue);
        return nullptr;
    }

    AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
    if (!Actor)
    {
        return nullptr;
    }

    Actor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
    Actor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Actor->SetActorScale3D(Scale);
    return Actor;
}

void UOmnisWorldContentSubsystem::BuildProductionWorld()
{
    if (bBuilt)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Static environment is authored and persisted by GenerateOmnisWorldContent.py.
    // Runtime only provides dynamic/interactive actors when the persistent map exists.
    const bool bPersistentWorld = World->GetName().Contains(TEXT("Omnis_OpenWorld"));

    if (!bPersistentWorld)
    {
        const TArray<FVector> BuildingLocations = {
            FVector(-3500,-2200,0), FVector(-1400,-2200,0), FVector(800,-2200,0),
            FVector(3000,-2200,0), FVector(-3500,0,0), FVector(3000,0,0),
            FVector(-3500,2200,0), FVector(-1400,2200,0), FVector(800,2200,0),
            FVector(3000,2200,0)
        };

        const TArray<FString> BuildingAssets = {
            AssetPath(TEXT("Architecture"),TEXT("Omnis_House")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Villa")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Mansion")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Apartment")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Office")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Hotel")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Hospital")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_School")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Bank")),
            AssetPath(TEXT("Architecture"),TEXT("Omnis_Nightclub"))
        };

        for (int32 Index=0; Index<BuildingLocations.Num(); ++Index)
        {
            SpawnStaticAsset(BuildingAssets[Index], BuildingLocations[Index]);
        }
    }

    if (AOmnisVehiclePawn* Sedan = World->SpawnActor<AOmnisVehiclePawn>(FVector(-800,-500,1), FRotator::ZeroRotator))
    {
        Sedan->ConfigureVehicle(AssetPath(TEXT("Vehicles"),TEXT("Omnis_Sedan")));
    }

    if (AOmnisVehiclePawn* Police = World->SpawnActor<AOmnisVehiclePawn>(FVector(-5000,3300,1), FRotator::ZeroRotator))
    {
        Police->ConfigureVehicle(AssetPath(TEXT("Vehicles"),TEXT("Omnis_Police_Car")));
    }

    if (AOmnisVehiclePawn* Ambulance = World->SpawnActor<AOmnisVehiclePawn>(FVector(2500,-2200,1), FRotator::ZeroRotator))
    {
        Ambulance->ConfigureVehicle(AssetPath(TEXT("Vehicles"),TEXT("Omnis_Ambulance")));
    }

    if (AOmnisAircraftPawn* Commercial = World->SpawnActor<AOmnisAircraftPawn>(FVector(0,11000,1400), FRotator::ZeroRotator))
    {
        Commercial->ConfigureAircraft(EOmnisAircraftType::Commercial, AssetPath(TEXT("Aviation"),TEXT("Omnis_Commercial_Jet")));
    }

    if (AOmnisAircraftPawn* Private = World->SpawnActor<AOmnisAircraftPawn>(FVector(3000,11000,1100), FRotator::ZeroRotator))
    {
        Private->ConfigureAircraft(EOmnisAircraftType::Private, AssetPath(TEXT("Aviation"),TEXT("Omnis_Private_Jet")));
    }

    if (AOmnisAircraftPawn* Helicopter = World->SpawnActor<AOmnisAircraftPawn>(FVector(6000,11000,900), FRotator::ZeroRotator))
    {
        Helicopter->ConfigureAircraft(EOmnisAircraftType::Helicopter, AssetPath(TEXT("Aviation"),TEXT("Omnis_Helicopter")));
    }

    if (AOmnisWildlifeActor* Dog = World->SpawnActor<AOmnisWildlifeActor>(FVector(-4500,-4800,1), FRotator::ZeroRotator))
    {
        Dog->ConfigureWildlifeAsset(AssetPath(TEXT("Animals"),TEXT("Omnis_Dog")));
    }

    if (AOmnisWildlifeActor* Horse = World->SpawnActor<AOmnisWildlifeActor>(FVector(4800,4800,1), FRotator::ZeroRotator))
    {
        Horse->ConfigureWildlifeAsset(AssetPath(TEXT("Animals"),TEXT("Omnis_Horse")));
    }

    bBuilt = true;
}
