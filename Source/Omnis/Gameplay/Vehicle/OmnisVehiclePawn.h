#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Gameplay/Vehicle/OmnisVehicleConditionComponent.h"
#include "OmnisVehiclePawn.generated.h"

class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisVehiclePawn : public APawn
{
    GENERATED_BODY()

public:
    AOmnisVehiclePawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle")
    void ConfigureVehicle(const FString& MeshAssetPath);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle")
    void ConfigureVehicleArchetype(EOmnisVehicleArchetype NewArchetype);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Environment")
    void SetTerrainContext(FName CellId, FName SurfaceId, float RainIntensity, float SaltExposure, float AmbientTemperatureC);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Environment")
    void SetWaterImmersion(float ImmersionFraction);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Environment")
    void SetRoadGradePercent(float GradePercent);

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Omnis|Vehicle|Persistence")
    FName VehiclePersistentId;

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Persistence")
    FOmnisVehicleConditionSnapshot ExportVehicleCondition() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Persistence")
    void ImportVehicleCondition(const FOmnisVehicleConditionSnapshot& Snapshot);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> VehicleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle")
    TObjectPtr<UOmnisVehicleConditionComponent> VehicleCondition;

private:
    void Throttle(float Value);
    void Steer(float Value);
    void BrakePressed();
    void BrakeReleased();
    void ApplyConfiguredMesh();

    UFUNCTION()
    void HandleVehicleImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(EditAnywhere, Category="Omnis|Vehicle")
    float EngineForce = 1800000.0f;

    UPROPERTY(EditAnywhere, Category="Omnis|Vehicle")
    float SteeringTorque = 9000000.0f;

    UPROPERTY(EditAnywhere, Category="Omnis|Vehicle")
    float BrakeForce = 2400000.0f;

    FString VehicleMeshPath;
    float ThrottleInput = 0.0f;
    float SteerInput = 0.0f;
    bool bBrake = false;
    FName TerrainCellId;
    FName TerrainSurfaceId;
    float TerrainRainIntensity = 0.0f;
    float TerrainSaltExposure = 0.0f;
    float AmbientTemperatureC = 25.0f;
    float RoadGradePercent = 0.0f;
};