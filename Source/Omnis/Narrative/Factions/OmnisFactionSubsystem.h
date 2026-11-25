#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisFactionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisSecurityResponseTier : uint8
{
    None,
    Patrol,
    MobileResponse,
    CoordinatedResponse
};

USTRUCT(BlueprintType)
struct FOmnisFactionTerritorySnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Factions")
    FName TerritoryId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Factions")
    int32 Heat = 0;
};

UCLASS()
class OMNIS_API UOmnisFactionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Factions")
    void AdjustStanding(FName FactionId, int32 Delta);

    UFUNCTION(BlueprintPure, Category="Omnis|Factions")
    int32 GetStanding(FName FactionId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Factions")
    TMap<FName, int32> ExportStandings() const { return Standings; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Factions")
    void ImportStandings(const TMap<FName, int32>& Values);

    UFUNCTION(BlueprintPure, Category="Omnis|Factions|Territory")
    FName GetDominantFactionForTerritory(FName TerritoryId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Factions|Territory")
    bool CanEnterTerritory(FName TerritoryId, bool bHasLocalContact) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Factions|Territory")
    bool RecordTerritoryIncident(
        FName TerritoryId,
        int32 HeatDelta,
        FName AffectedFactionId,
        int32 StandingDelta,
        bool bWitnessed);

    UFUNCTION(BlueprintCallable, Category="Omnis|Factions|Territory")
    void DecayTerritoryHeat(FName TerritoryId, int32 Amount);

    UFUNCTION(BlueprintPure, Category="Omnis|Factions|Territory")
    int32 GetTerritoryHeat(FName TerritoryId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Factions|Territory")
    EOmnisSecurityResponseTier GetSecurityResponseTier(FName TerritoryId) const;

    TArray<FOmnisFactionTerritorySnapshot> ExportTerritoryStates() const;
    void ImportTerritoryStates(const TArray<FOmnisFactionTerritorySnapshot>& Snapshots);

private:
    bool LoadTerritoryCatalog();

    UPROPERTY(SaveGame)
    TMap<FName, int32> Standings;

    UPROPERTY(SaveGame)
    TMap<FName, int32> TerritoryHeat;

    TMap<FName, FName> DominantFactionByTerritory;
    TMap<FName, int32> AccessStandingByTerritory;
    TSet<FName> ContactRequiredTerritories;

    int32 PatrolResponseThreshold = 15;
    int32 MobileResponseThreshold = 40;
    int32 CoordinatedResponseThreshold = 70;
};
