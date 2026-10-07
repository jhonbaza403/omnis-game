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
    if (bBuilt) return;

    UWorld* World = GetWorld();
    if (!World) return;

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

    SpawnStaticAsset(AssetPath(TEXT("Architecture"),TEXT("Omnis_Police_Station")), FVector(-5200,3600,0));
    SpawnStaticAsset(AssetPath(TEXT("Architecture"),TEXT("Omnis_Industrial")), FVector(5000,-3600,0));

    const FString RoadPath = AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Road_Segment"));
    for (int32 Index=-4; Index<=4; ++Index)
    {
        SpawnStaticAsset(RoadPath, FVector(Index*2000.0f,0,-10));
    }

    SpawnStaticAsset(AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Bridge")), FVector(0,5200,0));
    SpawnStaticAsset(AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Airport_Terminal")), FVector(0,9000,0));
    SpawnStaticAsset(AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Hangar")), FVector(5000,9000,0));
    SpawnStaticAsset(AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Runway_Segment")), FVector(0,11000,0), FVector(2.5f,1.0f,1.0f));
    SpawnStaticAsset(AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Dock")), FVector(5200,5200,0));
    SpawnStaticAsset(AssetPath(TEXT("Infrastructure"),TEXT("Omnis_Boat")), FVector(5200,5200,260));

    const FString Tree = AssetPath(TEXT("Nature"),TEXT("Omnis_Tree"));
    const FString Palm = AssetPath(TEXT("Nature"),TEXT("Omnis_Palm"));
    const FString Bush = AssetPath(TEXT("Nature"),TEXT("Omnis_Bush"));
    const FString Rock = AssetPath(TEXT("Nature"),TEXT("Omnis_Rock"));

    for (int32 Index=0; Index<24; ++Index)
    {
        const float Angle = FMath::DegreesToRadians(Index * 15.0f);
        const FVector Center(FMath::Cos(Angle)*6500.0f, FMath::Sin(Angle)*6500.0f, 0);
        SpawnStaticAsset(Tree, Center + FVector(180,80,0), FVector(2.0f));
        SpawnStaticAsset(Bush, Center + FVector(-180,-100,0), FVector(1.2f));
        SpawnStaticAsset(Rock, Center + FVector(300,250,0));
        if ((Index % 2) == 0)
        {
            SpawnStaticAsset(Palm, Center + FVector(420,-200,0), FVector(1.4f));
        }
    }

    if (AOmnisVehiclePawn* Sedan = World->SpawnActor<AOmnisVehiclePawn>(FVector(-800,-500,180), FRotator::ZeroRotator))
    {
        Sedan->ConfigureVehicle(AssetPath(TEXT("Vehicles"),TEXT("Omnis_Sedan")));
    }

    if (AOmnisVehiclePawn* Police = World->SpawnActor<AOmnisVehiclePawn>(FVector(-5000,3300,180), FRotator::ZeroRotator))
    {
        Police->ConfigureVehicle(AssetPath(TEXT("Vehicles"),TEXT("Omnis_Police_Car")));
    }

    if (AOmnisVehiclePawn* Ambulance = World->SpawnActor<AOmnisVehiclePawn>(FVector(2500,-2200,180), FRotator::ZeroRotator))
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

    if (AOmnisWildlifeActor* Dog = World->SpawnActor<AOmnisWildlifeActor>(FVector(-4500,-4800,120), FRotator::ZeroRotator))
    {
        Dog->ConfigureWildlifeAsset(AssetPath(TEXT("Animals"),TEXT("Omnis_Dog")));
    }

    if (AOmnisWildlifeActor* Horse = World->SpawnActor<AOmnisWildlifeActor>(FVector(4800,4800,120), FRotator::ZeroRotator))
    {
        Horse->ConfigureWildlifeAsset(AssetPath(TEXT("Animals"),TEXT("Omnis_Horse")));
    }

    bBuilt = true;
}
