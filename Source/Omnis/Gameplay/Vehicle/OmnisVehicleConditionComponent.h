#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisVehicleConditionComponent.generated.h"

UENUM(BlueprintType)
enum class EOmnisVehicleArchetype : uint8
{
    Motorcycle,
    Sedan,
    FourByFour,
    Boat
};

UENUM(BlueprintType)
enum class EOmnisVehicleDriveMode : uint8
{
    TwoWheelDrive,
    FourWheelDriveHigh,
    FourWheelDriveLow
};

USTRUCT(BlueprintType)
struct FOmnisVehicleConditionSnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    FName PersistentId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    EOmnisVehicleArchetype Archetype = EOmnisVehicleArchetype::Sedan;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    EOmnisVehicleDriveMode DriveMode = EOmnisVehicleDriveMode::TwoWheelDrive;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float FuelLiters = 0.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float EngineIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float TransmissionIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float SuspensionIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float BrakeIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float TireIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float CoolingIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float CorrosionIntegrity = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float EngineTemperatureC = 80.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float BrakeTemperatureC = 25.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float SteeringAlignmentDegrees = 0.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Vehicle")
    float DistanceDrivenKm = 0.0f;
};

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisVehicleConditionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisVehicleConditionComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetArchetype(EOmnisVehicleArchetype NewArchetype);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetDriveMode(EOmnisVehicleDriveMode NewMode);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetDriverInputs(float Throttle, float Brake);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetCargoLoadKg(float LoadKg);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetSurfaceGripScale(float GripScale);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetAmbientConditions(float AmbientTemperatureC, float SaltExposure);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetEngineLoadFactor(float LoadFactor);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void SetImmersionFraction(float Fraction);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    float Refuel(float RequestedLiters);

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetFuelCapacityLiters() const { return FuelCapacityLiters; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void ApplyImpact(float ImpactSpeedKph, FVector ImpactDirection);

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle|Condition")
    void RepairComponent(FName ComponentId, float RepairAmount);

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetPowertrainOutputScale() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetBrakeOutputScale() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetSteeringBiasDegrees() const { return SteeringAlignmentDegrees; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetTractionScale() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetTorqueMultiplier() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetBuoyancyLiftScale() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetFuelLiters() const { return FuelLiters; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetEngineTemperatureC() const { return EngineTemperatureC; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetBrakeTemperatureC() const { return BrakeTemperatureC; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetEngineIntegrity() const { return EngineIntegrity; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetSuspensionIntegrity() const { return SuspensionIntegrity; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    float GetBrakeIntegrity() const { return BrakeIntegrity; }

    UFUNCTION(BlueprintPure, Category="Omnis|Vehicle|Condition")
    bool IsBoat() const { return Archetype == EOmnisVehicleArchetype::Boat; }

    FOmnisVehicleConditionSnapshot ExportSnapshot(FName PersistentId) const;
    void ImportSnapshot(const FOmnisVehicleConditionSnapshot& Snapshot);

private:
    void ApplyArchetypeDefaults();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    EOmnisVehicleArchetype Archetype = EOmnisVehicleArchetype::Sedan;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    EOmnisVehicleDriveMode DriveMode = EOmnisVehicleDriveMode::TwoWheelDrive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float FuelLiters = 52.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float EngineIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float TransmissionIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float SuspensionIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float BrakeIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float TireIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float CoolingIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float CorrosionIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float EngineTemperatureC = 80.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float BrakeTemperatureC = 25.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float SteeringAlignmentDegrees = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Vehicle|Condition", meta=(AllowPrivateAccess="true"))
    float DistanceDrivenKm = 0.0f;

    float FuelCapacityLiters = 52.0f;
    float BaseFuelConsumptionLitersPerHour = 7.5f;
    float CargoLoadKg = 0.0f;
    float SurfaceGripScale = 1.0f;
    float AmbientTemperatureC = 25.0f;
    float SaltExposure = 0.0f;
    float EngineLoadFactor = 1.0f;
    float ImmersionFraction = 0.0f;
    float ThrottleInput = 0.0f;
    float BrakeInput = 0.0f;
    bool bProfileInitialized = false;
};
