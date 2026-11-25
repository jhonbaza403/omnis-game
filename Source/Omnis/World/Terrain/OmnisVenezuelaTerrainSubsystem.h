#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisVenezuelaTerrainSubsystem.generated.h"

class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FOmnisTerrainSurfaceResponse
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Terrain")
    float GripMultiplier = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category="Terrain")
    float RollingResistance = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Terrain")
    float MudDeformationRate = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Terrain")
    float CorrosionRatePerHour = 0.0f;
};

USTRUCT(BlueprintType)
struct FOmnisTerrainDeformationSnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Terrain")
    FName CellId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Terrain")
    float RutDepthCm = 0.0f;
};

USTRUCT()
struct FOmnisTerrainAltitudeSample
{
    GENERATED_BODY()

    UPROPERTY()
    float Progress = 0.0f;

    UPROPERTY()
    float AltitudeMeters = 0.0f;
};

UCLASS()
class OMNIS_API UOmnisVenezuelaTerrainSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintPure, Category="Omnis|World|Terrain")
    bool IsCatalogLoaded() const { return bCatalogLoaded; }

    UFUNCTION(BlueprintCallable, Category="Omnis|World|Terrain")
    bool GetAltitudeMeters(FName ProfileId, float Progress, float& OutAltitudeMeters) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|World|Terrain")
    bool GetGradePercent(
        FName ProfileId,
        float StartProgress,
        float EndProgress,
        float HorizontalDistanceMeters,
        float& OutGradePercent) const;

    UFUNCTION(BlueprintPure, Category="Omnis|World|Terrain")
    bool GetBiomeForRegion(FName RegionId, FName& OutBiomeId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|World|Terrain")
    bool CanRevealRegion(FName RegionId, FName TraversalEventId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|World|Terrain")
    bool GetSurfaceResponse(
        FName SurfaceId,
        float RainIntensity,
        float SaltExposure,
        FOmnisTerrainSurfaceResponse& OutResponse) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|World|Terrain")
    float RecordWheelPass(
        FName CellId,
        FName SurfaceId,
        float WheelLoadKg,
        float SpeedKph,
        float RainIntensity,
        float ContactSeconds);

    UFUNCTION(BlueprintPure, Category="Omnis|World|Terrain")
    float GetRutDepthCm(FName CellId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|World|Terrain")
    void ApplySurfaceMaterialParameters(
        UMaterialInstanceDynamic* Material,
        FName SurfaceId,
        float RainIntensity,
        float SaltExposure) const;

    TArray<FOmnisTerrainDeformationSnapshot> ExportDeformationSnapshots() const;
    void ImportDeformationSnapshots(const TArray<FOmnisTerrainDeformationSnapshot>& Snapshots);

private:
    struct FSurfaceData
    {
        float BaseGrip = 1.0f;
        float WetGripMultiplier = 1.0f;
        float RollingResistance = 0.0f;
        float MudDeformationRate = 0.0f;
        float CorrosionRatePerHour = 0.0f;
    };

    bool LoadCatalog();

    TMap<FName, TArray<FOmnisTerrainAltitudeSample>> AltitudeProfiles;
    TMap<FName, FName> BiomeByRegion;
    TMap<FName, FSurfaceData> SurfaceCatalog;
    TMap<FName, TArray<FName>> RevealEventsByRegion;
    TMap<FName, float> RutDepthByCell;

    bool bCatalogLoaded = false;
};
