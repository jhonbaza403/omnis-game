#include "World/Content/OmnisWorldContentSubsystem.h"

#include "AI/NPC/OmnisNPCCharacter.h"
#include "AI/NPC/OmnisCharacterLifeSubsystem.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Gameplay/Aviation/OmnisAircraftPawn.h"
#include "Gameplay/Vehicle/OmnisVehiclePawn.h"
#include "World/Wildlife/OmnisWildlifeActor.h"

namespace
{
    struct FOmnisProductionCitizenSeed
    {
        const TCHAR* PersistentId;
        const TCHAR* CharacterId;
        const TCHAR* DisplayName;
        const TCHAR* Biography;
        const TCHAR* OccupationId;
        const TCHAR* HomeLocationId;
        const TCHAR* WorkLocationId;
        FVector Location;
        FVector WorkOffset;
        float WorkStartHour;
        float WorkEndHour;
        float RestStartHour;
        float RestEndHour;
        int32 WorkDaysMask;
    };

    void SpawnProductionCitizen(UWorld* World, const FOmnisProductionCitizenSeed& Seed)
    {
        if (!World || !World->GetAuthGameMode())
        {
            return;
        }

        const FName StableId(Seed.PersistentId);
        UOmnisCharacterLifeSubsystem* Life =
            World->GetSubsystem<UOmnisCharacterLifeSubsystem>();
        const FVector HomePosition = Seed.Location;
        const FVector WorkPosition = Seed.Location + Seed.WorkOffset;
        if (!Life ||
            !Life->RegisterLocation(FName(Seed.HomeLocationId), HomePosition) ||
            !Life->RegisterLocation(FName(Seed.WorkLocationId), WorkPosition))
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS citizen '%s' could not register home/work locations."),
                Seed.PersistentId);
            return;
        }

        for (TActorIterator<AOmnisNPCCharacter> It(World); It; ++It)
        {
            if (*It && (*It)->GetPersistentId() == StableId)
            {
                return;
            }
        }

        const FTransform SpawnTransform(FRotator::ZeroRotator, Seed.Location);
        AOmnisNPCCharacter* NPC = World->SpawnActorDeferred<AOmnisNPCCharacter>(
            AOmnisNPCCharacter::StaticClass(),
            SpawnTransform);
        if (!NPC)
        {
            return;
        }

        NPC->PersistentId = StableId;
        NPC->CharacterId = FName(Seed.CharacterId);
        NPC->CharacterDisplayName = FText::FromString(Seed.DisplayName);
        NPC->CharacterBiography = FText::FromString(Seed.Biography);
        NPC->OccupationId = FName(Seed.OccupationId);
        NPC->HomeLocationId = FName(Seed.HomeLocationId);
        NPC->WorkLocationId = FName(Seed.WorkLocationId);
        NPC->WorkLocationOffset = Seed.WorkOffset;
        NPC->WorkStartHour = Seed.WorkStartHour;
        NPC->WorkEndHour = Seed.WorkEndHour;
        NPC->RestStartHour = Seed.RestStartHour;
        NPC->RestEndHour = Seed.RestEndHour;
        NPC->WorkDaysMask = Seed.WorkDaysMask;
        NPC->FinishSpawning(SpawnTransform);
    }

    void SpawnProductionPopulation(UWorld* World)
    {
        static const FOmnisProductionCitizenSeed Citizens[] = {
            {TEXT("Citizen.OpenWorld.Civilian.001"), TEXT("NPC.Civilian"), TEXT("Elena Ríos"), TEXT("Residente del barrio norte; conoce a los comerciantes y ayuda a coordinar las actividades vecinales."), TEXT("Occupation.CommunityVolunteer"), TEXT("Home.NorthQuarter.001"), TEXT("Work.CommunityCenter"), FVector(-2600.0f, -900.0f, 115.0f), FVector(900.0f, 0.0f, 0.0f), 7.0f, 17.0f, 21.0f, 6.0f, 0x1F},
            {TEXT("Citizen.OpenWorld.Civilian.002"), TEXT("NPC.Civilian"), TEXT("Mateo Cruz"), TEXT("Trabaja en mantenimiento urbano; suele recorrer el distrito para reparar servicios y mobiliario."), TEXT("Occupation.MaintenanceWorker"), TEXT("Home.EastQuarter.002"), TEXT("Work.MunicipalDepot"), FVector(-1200.0f, 1500.0f, 115.0f), FVector(0.0f, 1000.0f, 0.0f), 8.0f, 18.0f, 22.0f, 6.0f, 0x3F},
            {TEXT("Citizen.OpenWorld.Shopkeeper.001"), TEXT("NPC.Shopkeeper"), TEXT("Lucía Navarro"), TEXT("Comerciante local; abre temprano, atiende a los vecinos y conoce los movimientos del mercado."), TEXT("Occupation.Shopkeeper"), TEXT("Home.MarketQuarter.001"), TEXT("Work.Market.Store.001"), FVector(500.0f, -1400.0f, 115.0f), FVector(1000.0f, 200.0f, 0.0f), 6.0f, 16.0f, 22.0f, 5.0f, 0x3F},
            {TEXT("Citizen.OpenWorld.Police.001"), TEXT("NPC.PoliceOfficer"), TEXT("Daniel Vega"), TEXT("Agente de patrulla encargado de mantener la seguridad del sector y responder a incidentes."), TEXT("Occupation.PoliceOfficer"), TEXT("Home.CivicQuarter.001"), TEXT("Work.PoliceStation"), FVector(1900.0f, 700.0f, 115.0f), FVector(-900.0f, 0.0f, 0.0f), 6.0f, 18.0f, 23.0f, 6.0f, 0x7F},
            {TEXT("Citizen.OpenWorld.Medic.001"), TEXT("NPC.Medic"), TEXT("Sara Molina"), TEXT("Personal sanitario del hospital; atiende emergencias y cubre turnos de guardia."), TEXT("Occupation.Medic"), TEXT("Home.HospitalQuarter.001"), TEXT("Work.Hospital.Emergency"), FVector(3200.0f, -1700.0f, 115.0f), FVector(0.0f, -900.0f, 0.0f), 7.0f, 19.0f, 23.0f, 7.0f, 0x7F},
            {TEXT("Citizen.OpenWorld.Pilot.001"), TEXT("NPC.Pilot"), TEXT("Andrés Salas"), TEXT("Piloto civil que realiza vuelos de transporte y revisa las aeronaves antes de cada jornada."), TEXT("Occupation.Pilot"), TEXT("Home.AirportQuarter.001"), TEXT("Work.Airport.Hangar"), FVector(4200.0f, 1900.0f, 115.0f), FVector(900.0f, 500.0f, 0.0f), 5.0f, 15.0f, 20.0f, 5.0f, 0x1F}
        };

        for (const FOmnisProductionCitizenSeed& Citizen : Citizens)
        {
            SpawnProductionCitizen(World, Citizen);
        }
    }

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
    if (!World || !World->GetAuthGameMode())
    {
        return;
    }

    // Static environment is authored and persisted by GenerateOmnisWorldContent.py.
    // Runtime only provides dynamic/interactive actors when the persistent map exists.
    const bool bPersistentWorld = World->GetName().Contains(TEXT("Omnis_OpenWorld"));
    if (bPersistentWorld)
    {
        SpawnProductionPopulation(World);
    }

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
