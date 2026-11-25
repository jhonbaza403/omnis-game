#include "World/Terrain/OmnisVenezuelaTerrainSubsystem.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UOmnisVenezuelaTerrainSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bCatalogLoaded = LoadCatalog();
    if (!bCatalogLoaded)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("OMNIS terrain catalog was not loaded; regional terrain queries will use safe fallbacks."));
    }
}

bool UOmnisVenezuelaTerrainSubsystem::LoadCatalog()
{
    const FString CatalogPath = FPaths::Combine(
        FPaths::ProjectContentDir(),
        TEXT("Omnis/Data/venezuela-terrain-profile-v1.json"));

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *CatalogPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("OMNIS terrain catalog missing: %s"), *CatalogPath);
        return false;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("OMNIS terrain catalog has invalid JSON."));
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Profiles = nullptr;
    if (!Root->TryGetArrayField(TEXT("profiles"), Profiles) || !Profiles)
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& Value : *Profiles)
    {
        const TSharedPtr<FJsonObject> Profile = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!Profile.IsValid())
        {
            continue;
        }

        FString Id;
        const TArray<TSharedPtr<FJsonValue>>* Samples = nullptr;
        if (!Profile->TryGetStringField(TEXT("id"), Id) ||
            !Profile->TryGetArrayField(TEXT("samples"), Samples) ||
            !Samples || Samples->Num() < 2)
        {
            continue;
        }

        TArray<FOmnisTerrainAltitudeSample> ParsedSamples;
        for (const TSharedPtr<FJsonValue>& SampleValue : *Samples)
        {
            const TSharedPtr<FJsonObject> Sample = SampleValue.IsValid()
                ? SampleValue->AsObject()
                : nullptr;
            if (!Sample.IsValid())
            {
                continue;
            }

            double Progress = 0.0;
            double Altitude = 0.0;
            if (!Sample->TryGetNumberField(TEXT("progress"), Progress) ||
                !Sample->TryGetNumberField(TEXT("altitude_m"), Altitude) ||
                !FMath::IsFinite(Progress) ||
                !FMath::IsFinite(Altitude) ||
                Progress < 0.0 || Progress > 1.0 ||
                Altitude < -5.0 || Altitude > 3500.0)
            {
                ParsedSamples.Reset();
                break;
            }

            FOmnisTerrainAltitudeSample Parsed;
            Parsed.Progress = static_cast<float>(Progress);
            Parsed.AltitudeMeters = static_cast<float>(Altitude);
            ParsedSamples.Add(Parsed);
        }

        ParsedSamples.Sort([](const FOmnisTerrainAltitudeSample& A, const FOmnisTerrainAltitudeSample& B)
        {
            return A.Progress < B.Progress;
        });

        if (ParsedSamples.Num() >= 2 &&
            FMath::IsNearlyZero(ParsedSamples[0].Progress) &&
            FMath::IsNearlyEqual(ParsedSamples.Last().Progress, 1.0f))
        {
            AltitudeProfiles.Add(FName(*Id), MoveTemp(ParsedSamples));
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* Biomes = nullptr;
    if (Root->TryGetArrayField(TEXT("biomes"), Biomes) && Biomes)
    {
        for (const TSharedPtr<FJsonValue>& Value : *Biomes)
        {
            const TSharedPtr<FJsonObject> Biome = Value.IsValid() ? Value->AsObject() : nullptr;
            if (!Biome.IsValid())
            {
                continue;
            }

            FString BiomeId;
            const TArray<TSharedPtr<FJsonValue>>* Regions = nullptr;
            if (!Biome->TryGetStringField(TEXT("id"), BiomeId) ||
                !Biome->TryGetArrayField(TEXT("regions"), Regions) ||
                !Regions)
            {
                continue;
            }

            for (const TSharedPtr<FJsonValue>& RegionValue : *Regions)
            {
                const FString RegionId = RegionValue->AsString();
                if (!RegionId.IsEmpty())
                {
                    BiomeByRegion.Add(FName(*RegionId), FName(*BiomeId));
                }
            }
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* Surfaces = nullptr;
    if (Root->TryGetArrayField(TEXT("surfaces"), Surfaces) && Surfaces)
    {
        for (const TSharedPtr<FJsonValue>& Value : *Surfaces)
        {
            const TSharedPtr<FJsonObject> Surface = Value.IsValid() ? Value->AsObject() : nullptr;
            if (!Surface.IsValid())
            {
                continue;
            }

            FString Id;
            if (!Surface->TryGetStringField(TEXT("id"), Id))
            {
                continue;
            }

            FSurfaceData Data;
            double Number = 0.0;
            if (Surface->TryGetNumberField(TEXT("base_grip"), Number))
            {
                Data.BaseGrip = FMath::Clamp(static_cast<float>(Number), 0.05f, 1.0f);
            }
            if (Surface->TryGetNumberField(TEXT("wet_grip_multiplier"), Number))
            {
                Data.WetGripMultiplier = FMath::Clamp(static_cast<float>(Number), 0.05f, 1.0f);
            }
            if (Surface->TryGetNumberField(TEXT("rolling_resistance"), Number))
            {
                Data.RollingResistance = FMath::Max(0.0f, static_cast<float>(Number));
            }
            if (Surface->TryGetNumberField(TEXT("mud_deformation_rate"), Number))
            {
                Data.MudDeformationRate = FMath::Max(0.0f, static_cast<float>(Number));
            }
            if (Surface->TryGetNumberField(TEXT("corrosion_rate_per_hour"), Number))
            {
                Data.CorrosionRatePerHour = FMath::Max(0.0f, static_cast<float>(Number));
            }
            SurfaceCatalog.Add(FName(*Id), Data);
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* Gates = nullptr;
    if (Root->TryGetArrayField(TEXT("reveal_gates"), Gates) && Gates)
    {
        for (const TSharedPtr<FJsonValue>& Value : *Gates)
        {
            const TSharedPtr<FJsonObject> Gate = Value.IsValid() ? Value->AsObject() : nullptr;
            if (!Gate.IsValid())
            {
                continue;
            }

            FString RegionId;
            const TArray<TSharedPtr<FJsonValue>>* Events = nullptr;
            if (!Gate->TryGetStringField(TEXT("region_id"), RegionId) ||
                !Gate->TryGetArrayField(TEXT("events"), Events) ||
                !Events)
            {
                continue;
            }

            TArray<FName> ParsedEvents;
            for (const TSharedPtr<FJsonValue>& EventValue : *Events)
            {
                const FString EventId = EventValue->AsString();
                if (!EventId.IsEmpty())
                {
                    ParsedEvents.AddUnique(FName(*EventId));
                }
            }
            if (!ParsedEvents.IsEmpty())
            {
                RevealEventsByRegion.Add(FName(*RegionId), MoveTemp(ParsedEvents));
            }
        }
    }

    return AltitudeProfiles.Num() > 0 &&
        BiomeByRegion.Num() > 0 &&
        SurfaceCatalog.Num() > 0;
}

bool UOmnisVenezuelaTerrainSubsystem::GetAltitudeMeters(
    FName ProfileId,
    float Progress,
    float& OutAltitudeMeters) const
{
    const TArray<FOmnisTerrainAltitudeSample>* Samples = AltitudeProfiles.Find(ProfileId);
    if (!Samples || Samples->Num() < 2 || !FMath::IsFinite(Progress))
    {
        return false;
    }

    const float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
    for (int32 Index = 1; Index < Samples->Num(); ++Index)
    {
        const FOmnisTerrainAltitudeSample& Previous = (*Samples)[Index - 1];
        const FOmnisTerrainAltitudeSample& Current = (*Samples)[Index];
        if (ClampedProgress <= Current.Progress)
        {
            const float SegmentSize = Current.Progress - Previous.Progress;
            const float Alpha = SegmentSize > KINDA_SMALL_NUMBER
                ? (ClampedProgress - Previous.Progress) / SegmentSize
                : 0.0f;
            OutAltitudeMeters = FMath::Lerp(
                Previous.AltitudeMeters,
                Current.AltitudeMeters,
                FMath::Clamp(Alpha, 0.0f, 1.0f));
            return true;
        }
    }

    OutAltitudeMeters = Samples->Last().AltitudeMeters;
    return true;
}


bool UOmnisVenezuelaTerrainSubsystem::GetGradePercent(
    FName ProfileId,
    float StartProgress,
    float EndProgress,
    float HorizontalDistanceMeters,
    float& OutGradePercent) const
{
    if (!FMath::IsFinite(StartProgress) ||
        !FMath::IsFinite(EndProgress) ||
        !FMath::IsFinite(HorizontalDistanceMeters) ||
        HorizontalDistanceMeters <= KINDA_SMALL_NUMBER ||
        EndProgress <= StartProgress)
    {
        return false;
    }

    float StartAltitudeMeters = 0.0f;
    float EndAltitudeMeters = 0.0f;
    if (!GetAltitudeMeters(ProfileId, StartProgress, StartAltitudeMeters) ||
        !GetAltitudeMeters(ProfileId, EndProgress, EndAltitudeMeters))
    {
        return false;
    }

    OutGradePercent = FMath::Clamp(
        ((EndAltitudeMeters - StartAltitudeMeters) / HorizontalDistanceMeters) * 100.0f,
        -60.0f,
        60.0f);
    return true;
}

bool UOmnisVenezuelaTerrainSubsystem::GetBiomeForRegion(
    FName RegionId,
    FName& OutBiomeId) const
{
    const FName* Found = BiomeByRegion.Find(RegionId);
    if (!Found)
    {
        return false;
    }
    OutBiomeId = *Found;
    return true;
}

bool UOmnisVenezuelaTerrainSubsystem::CanRevealRegion(
    FName RegionId,
    FName TraversalEventId) const
{
    const TArray<FName>* Events = RevealEventsByRegion.Find(RegionId);
    return Events && Events->Contains(TraversalEventId);
}

bool UOmnisVenezuelaTerrainSubsystem::GetSurfaceResponse(
    FName SurfaceId,
    float RainIntensity,
    float SaltExposure,
    FOmnisTerrainSurfaceResponse& OutResponse) const
{
    const FSurfaceData* Surface = SurfaceCatalog.Find(SurfaceId);
    if (!Surface)
    {
        return false;
    }

    const float Rain = FMath::Clamp(FMath::IsFinite(RainIntensity) ? RainIntensity : 0.0f, 0.0f, 1.0f);
    const float Salt = FMath::Clamp(FMath::IsFinite(SaltExposure) ? SaltExposure : 0.0f, 0.0f, 1.0f);
    OutResponse.GripMultiplier = FMath::Lerp(
        Surface->BaseGrip,
        Surface->BaseGrip * Surface->WetGripMultiplier,
        Rain);
    OutResponse.RollingResistance = Surface->RollingResistance * (1.0f + Rain * 0.65f);
    OutResponse.MudDeformationRate = Surface->MudDeformationRate * (0.25f + Rain);
    OutResponse.CorrosionRatePerHour = Surface->CorrosionRatePerHour * Salt;
    return true;
}

float UOmnisVenezuelaTerrainSubsystem::RecordWheelPass(
    FName CellId,
    FName SurfaceId,
    float WheelLoadKg,
    float SpeedKph,
    float RainIntensity,
    float ContactSeconds)
{
    if (CellId.IsNone() || (GetWorld() && GetWorld()->GetNetMode() == NM_Client))
    {
        return 0.0f;
    }

    FOmnisTerrainSurfaceResponse Response;
    if (!GetSurfaceResponse(SurfaceId, RainIntensity, 0.0f, Response) ||
        Response.MudDeformationRate <= 0.0f)
    {
        return GetRutDepthCm(CellId);
    }

    const float LoadFactor = FMath::Clamp(
        FMath::Max(0.0f, WheelLoadKg) / 450.0f,
        0.25f,
        3.0f);
    const float SpeedFactor = FMath::Clamp(
        0.35f + FMath::Max(0.0f, SpeedKph) / 80.0f,
        0.35f,
        1.75f);
    const float SafeContactSeconds = FMath::Clamp(FMath::IsFinite(ContactSeconds) ? ContactSeconds : 0.0f, 0.0f, 0.25f);
    const float DepthDelta = Response.MudDeformationRate * LoadFactor * SpeedFactor * 8.0f * SafeContactSeconds;
    float& Depth = RutDepthByCell.FindOrAdd(CellId);
    Depth = FMath::Clamp(Depth + DepthDelta, 0.0f, 45.0f);
    return Depth;
}

float UOmnisVenezuelaTerrainSubsystem::GetRutDepthCm(FName CellId) const
{
    return RutDepthByCell.FindRef(CellId);
}

void UOmnisVenezuelaTerrainSubsystem::ApplySurfaceMaterialParameters(
    UMaterialInstanceDynamic* Material,
    FName SurfaceId,
    float RainIntensity,
    float SaltExposure) const
{
    if (!Material)
    {
        return;
    }

    FOmnisTerrainSurfaceResponse Response;
    if (!GetSurfaceResponse(SurfaceId, RainIntensity, SaltExposure, Response))
    {
        return;
    }

    Material->SetScalarParameterValue(
        TEXT("OMNIS_SurfaceGrip"),
        Response.GripMultiplier);
    Material->SetScalarParameterValue(
        TEXT("OMNIS_Wetness"),
        FMath::Clamp(RainIntensity, 0.0f, 1.0f));
    Material->SetScalarParameterValue(
        TEXT("OMNIS_SaltExposure"),
        FMath::Clamp(SaltExposure, 0.0f, 1.0f));
    Material->SetScalarParameterValue(
        TEXT("OMNIS_MudDeformation"),
        Response.MudDeformationRate);
}

TArray<FOmnisTerrainDeformationSnapshot>
UOmnisVenezuelaTerrainSubsystem::ExportDeformationSnapshots() const
{
    TArray<FOmnisTerrainDeformationSnapshot> Result;
    Result.Reserve(RutDepthByCell.Num());
    for (const TPair<FName, float>& Pair : RutDepthByCell)
    {
        FOmnisTerrainDeformationSnapshot Snapshot;
        Snapshot.CellId = Pair.Key;
        Snapshot.RutDepthCm = FMath::Clamp(Pair.Value, 0.0f, 45.0f);
        Result.Add(Snapshot);
    }
    Result.Sort([](const FOmnisTerrainDeformationSnapshot& A, const FOmnisTerrainDeformationSnapshot& B)
    {
        return A.CellId.Compare(B.CellId) < 0;
    });
    return Result;
}

void UOmnisVenezuelaTerrainSubsystem::ImportDeformationSnapshots(
    const TArray<FOmnisTerrainDeformationSnapshot>& Snapshots)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }

    RutDepthByCell.Reset();
    for (const FOmnisTerrainDeformationSnapshot& Snapshot : Snapshots)
    {
        if (!Snapshot.CellId.IsNone() && FMath::IsFinite(Snapshot.RutDepthCm))
        {
            RutDepthByCell.Add(
                Snapshot.CellId,
                FMath::Clamp(Snapshot.RutDepthCm, 0.0f, 45.0f));
        }
    }
}
