#include "Narrative/Factions/OmnisFactionSubsystem.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UOmnisFactionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (!LoadTerritoryCatalog())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("OMNIS faction territory catalog was not loaded; territory access queries will fail closed."));
    }
}

bool UOmnisFactionSubsystem::LoadTerritoryCatalog()
{
    const FString CatalogPath = FPaths::Combine(
        FPaths::ProjectContentDir(),
        TEXT("Omnis/Data/venezuela-factions-v1.json"));

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *CatalogPath))
    {
        return false;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Territories = nullptr;
    if (!Root->TryGetArrayField(TEXT("territories"), Territories) || !Territories)
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& Value : *Territories)
    {
        const TSharedPtr<FJsonObject> Territory = Value.IsValid()
            ? Value->AsObject()
            : nullptr;
        if (!Territory.IsValid())
        {
            continue;
        }

        FString TerritoryId;
        FString FactionId;
        double Threshold = 0.0;
        bool bRequiresContact = false;
        if (!Territory->TryGetStringField(TEXT("id"), TerritoryId) ||
            !Territory->TryGetStringField(TEXT("dominant_faction"), FactionId))
        {
            continue;
        }

        Territory->TryGetNumberField(TEXT("access_standing_threshold"), Threshold);
        Territory->TryGetBoolField(TEXT("requires_local_contact"), bRequiresContact);

        const FName TerritoryName(*TerritoryId);
        if (TerritoryName.IsNone())
        {
            continue;
        }

        DominantFactionByTerritory.Add(TerritoryName, FName(*FactionId));
        AccessStandingByTerritory.Add(
            TerritoryName,
            FMath::Clamp(static_cast<int32>(Threshold), -100, 100));
        if (bRequiresContact)
        {
            ContactRequiredTerritories.Add(TerritoryName);
        }
    }

    const TSharedPtr<FJsonObject>* Thresholds = nullptr;
    if (Root->TryGetObjectField(TEXT("response_thresholds"), Thresholds) &&
        Thresholds && Thresholds->IsValid())
    {
        double Number = 0.0;
        if ((*Thresholds)->TryGetNumberField(TEXT("patrol_from"), Number))
        {
            PatrolResponseThreshold = FMath::Clamp(static_cast<int32>(Number), 1, 100);
        }
        if ((*Thresholds)->TryGetNumberField(TEXT("mobile_units_from"), Number))
        {
            MobileResponseThreshold = FMath::Clamp(static_cast<int32>(Number), PatrolResponseThreshold + 1, 100);
        }
        if ((*Thresholds)->TryGetNumberField(TEXT("coordinated_response_from"), Number))
        {
            CoordinatedResponseThreshold = FMath::Clamp(static_cast<int32>(Number), MobileResponseThreshold + 1, 100);
        }
    }

    return DominantFactionByTerritory.Num() > 0;
}

void UOmnisFactionSubsystem::AdjustStanding(FName FactionId, int32 Delta)
{
    if ((GetWorld() && GetWorld()->GetNetMode() == NM_Client) ||
        FactionId.IsNone() || Delta == 0)
    {
        return;
    }

    Standings.FindOrAdd(FactionId) = FMath::Clamp(
        Standings.FindRef(FactionId) + Delta,
        -100,
        100);
}

int32 UOmnisFactionSubsystem::GetStanding(FName FactionId) const
{
    return FMath::Clamp(Standings.FindRef(FactionId), -100, 100);
}

void UOmnisFactionSubsystem::ImportStandings(const TMap<FName, int32>& Values)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }

    Standings.Reset();
    for (const TPair<FName, int32>& Pair : Values)
    {
        if (!Pair.Key.IsNone())
        {
            Standings.Add(Pair.Key, FMath::Clamp(Pair.Value, -100, 100));
        }
    }
}

FName UOmnisFactionSubsystem::GetDominantFactionForTerritory(FName TerritoryId) const
{
    return DominantFactionByTerritory.FindRef(TerritoryId);
}

bool UOmnisFactionSubsystem::CanEnterTerritory(
    FName TerritoryId,
    bool bHasLocalContact) const
{
    const FName* FactionId = DominantFactionByTerritory.Find(TerritoryId);
    const int32* RequiredStanding = AccessStandingByTerritory.Find(TerritoryId);
    if (!FactionId || !RequiredStanding)
    {
        return false;
    }

    if (ContactRequiredTerritories.Contains(TerritoryId) && bHasLocalContact)
    {
        return true;
    }
    return GetStanding(*FactionId) >= *RequiredStanding;
}

bool UOmnisFactionSubsystem::RecordTerritoryIncident(
    FName TerritoryId,
    int32 HeatDelta,
    FName AffectedFactionId,
    int32 StandingDelta,
    bool bWitnessed)
{
    if ((GetWorld() && GetWorld()->GetNetMode() == NM_Client) ||
        !DominantFactionByTerritory.Contains(TerritoryId))
    {
        return false;
    }

    const float WitnessScale = bWitnessed ? 1.0f : 0.5f;
    const int32 EffectiveHeatDelta = FMath::RoundToInt(
        static_cast<float>(HeatDelta) * WitnessScale);
    TerritoryHeat.FindOrAdd(TerritoryId) = FMath::Clamp(
        TerritoryHeat.FindRef(TerritoryId) + EffectiveHeatDelta,
        0,
        100);

    if (bWitnessed && !AffectedFactionId.IsNone() && StandingDelta != 0)
    {
        AdjustStanding(AffectedFactionId, StandingDelta);
    }
    return true;
}

void UOmnisFactionSubsystem::DecayTerritoryHeat(FName TerritoryId, int32 Amount)
{
    if ((GetWorld() && GetWorld()->GetNetMode() == NM_Client) ||
        TerritoryId.IsNone() || Amount <= 0)
    {
        return;
    }

    const int32 Current = TerritoryHeat.FindRef(TerritoryId);
    TerritoryHeat.Add(TerritoryId, FMath::Max(0, Current - Amount));
}

int32 UOmnisFactionSubsystem::GetTerritoryHeat(FName TerritoryId) const
{
    return FMath::Clamp(TerritoryHeat.FindRef(TerritoryId), 0, 100);
}

EOmnisSecurityResponseTier UOmnisFactionSubsystem::GetSecurityResponseTier(
    FName TerritoryId) const
{
    const int32 Heat = GetTerritoryHeat(TerritoryId);
    if (Heat >= CoordinatedResponseThreshold)
    {
        return EOmnisSecurityResponseTier::CoordinatedResponse;
    }
    if (Heat >= MobileResponseThreshold)
    {
        return EOmnisSecurityResponseTier::MobileResponse;
    }
    if (Heat >= PatrolResponseThreshold)
    {
        return EOmnisSecurityResponseTier::Patrol;
    }
    return EOmnisSecurityResponseTier::None;
}

TArray<FOmnisFactionTerritorySnapshot>
UOmnisFactionSubsystem::ExportTerritoryStates() const
{
    TArray<FOmnisFactionTerritorySnapshot> Result;
    Result.Reserve(TerritoryHeat.Num());
    for (const TPair<FName, int32>& Pair : TerritoryHeat)
    {
        FOmnisFactionTerritorySnapshot Snapshot;
        Snapshot.TerritoryId = Pair.Key;
        Snapshot.Heat = FMath::Clamp(Pair.Value, 0, 100);
        Result.Add(Snapshot);
    }
    Result.Sort([](const FOmnisFactionTerritorySnapshot& A, const FOmnisFactionTerritorySnapshot& B)
    {
        return A.TerritoryId.Compare(B.TerritoryId) < 0;
    });
    return Result;
}

void UOmnisFactionSubsystem::ImportTerritoryStates(
    const TArray<FOmnisFactionTerritorySnapshot>& Snapshots)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }

    TerritoryHeat.Reset();
    for (const FOmnisFactionTerritorySnapshot& Snapshot : Snapshots)
    {
        if (!Snapshot.TerritoryId.IsNone() &&
            DominantFactionByTerritory.Contains(Snapshot.TerritoryId))
        {
            TerritoryHeat.Add(
                Snapshot.TerritoryId,
                FMath::Clamp(Snapshot.Heat, 0, 100));
        }
    }
}
