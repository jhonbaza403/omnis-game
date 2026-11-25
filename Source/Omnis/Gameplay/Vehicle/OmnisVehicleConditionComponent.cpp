#include "Gameplay/Vehicle/OmnisVehicleConditionComponent.h"

#include "GameFramework/Actor.h"

UOmnisVehicleConditionComponent::UOmnisVehicleConditionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UOmnisVehicleConditionComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!bProfileInitialized)
    {
        ApplyArchetypeDefaults();
    }
}

void UOmnisVehicleConditionComponent::SetArchetype(EOmnisVehicleArchetype NewArchetype)
{
    Archetype = NewArchetype;
    ApplyArchetypeDefaults();
}

void UOmnisVehicleConditionComponent::ApplyArchetypeDefaults()
{
    switch (Archetype)
    {
    case EOmnisVehicleArchetype::Motorcycle:
        FuelCapacityLiters = 14.0f;
        BaseFuelConsumptionLitersPerHour = 2.8f;
        CargoLoadKg = 0.0f;
        break;
    case EOmnisVehicleArchetype::FourByFour:
        FuelCapacityLiters = 82.0f;
        BaseFuelConsumptionLitersPerHour = 12.5f;
        CargoLoadKg = 0.0f;
        break;
    case EOmnisVehicleArchetype::Boat:
        FuelCapacityLiters = 150.0f;
        BaseFuelConsumptionLitersPerHour = 15.0f;
        CargoLoadKg = 0.0f;
        break;
    case EOmnisVehicleArchetype::Sedan:
    default:
        FuelCapacityLiters = 52.0f;
        BaseFuelConsumptionLitersPerHour = 7.5f;
        CargoLoadKg = 0.0f;
        break;
    }

    FuelLiters = bProfileInitialized
        ? FMath::Clamp(FuelLiters, 0.0f, FuelCapacityLiters)
        : FuelCapacityLiters;
    if (Archetype != EOmnisVehicleArchetype::FourByFour)
    {
        DriveMode = EOmnisVehicleDriveMode::TwoWheelDrive;
    }
    bProfileInitialized = true;
}

void UOmnisVehicleConditionComponent::SetDriveMode(EOmnisVehicleDriveMode NewMode)
{
    DriveMode = Archetype == EOmnisVehicleArchetype::FourByFour
        ? NewMode
        : EOmnisVehicleDriveMode::TwoWheelDrive;
}

void UOmnisVehicleConditionComponent::SetDriverInputs(float Throttle, float Brake)
{
    ThrottleInput = FMath::Clamp(FMath::IsFinite(Throttle) ? Throttle : 0.0f, -1.0f, 1.0f);
    BrakeInput = FMath::Clamp(FMath::IsFinite(Brake) ? Brake : 0.0f, 0.0f, 1.0f);
}

void UOmnisVehicleConditionComponent::SetCargoLoadKg(float LoadKg)
{
    CargoLoadKg = FMath::Clamp(FMath::IsFinite(LoadKg) ? LoadKg : 0.0f, 0.0f, 2500.0f);
}

void UOmnisVehicleConditionComponent::SetSurfaceGripScale(float GripScale)
{
    SurfaceGripScale = FMath::Clamp(FMath::IsFinite(GripScale) ? GripScale : 1.0f, 0.1f, 1.5f);
}

void UOmnisVehicleConditionComponent::SetAmbientConditions(
    float NewAmbientTemperatureC,
    float NewSaltExposure)
{
    AmbientTemperatureC = FMath::Clamp(
        FMath::IsFinite(NewAmbientTemperatureC) ? NewAmbientTemperatureC : 25.0f,
        -20.0f,
        55.0f);
    SaltExposure = FMath::Clamp(
        FMath::IsFinite(NewSaltExposure) ? NewSaltExposure : 0.0f,
        0.0f,
        1.0f);
}

void UOmnisVehicleConditionComponent::SetEngineLoadFactor(float LoadFactor)
{
    EngineLoadFactor = FMath::Clamp(
        FMath::IsFinite(LoadFactor) ? LoadFactor : 1.0f,
        0.5f,
        2.5f);
}

void UOmnisVehicleConditionComponent::SetRoadGradePercent(float GradePercent)
{
    RoadGradePercent = FMath::Clamp(
        FMath::IsFinite(GradePercent) ? GradePercent : 0.0f,
        -35.0f,
        35.0f);
}

float UOmnisVehicleConditionComponent::Refuel(float RequestedLiters)
{
    if ((GetOwner() && GetOwner()->GetNetMode() == NM_Client) ||
        !FMath::IsFinite(RequestedLiters) || RequestedLiters <= 0.0f)
    {
        return 0.0f;
    }
    const float Accepted = FMath::Min(RequestedLiters, FuelCapacityLiters - FuelLiters);
    FuelLiters = FMath::Clamp(FuelLiters + Accepted, 0.0f, FuelCapacityLiters);
    return Accepted;
}

void UOmnisVehicleConditionComponent::SetImmersionFraction(float Fraction)
{
    ImmersionFraction = FMath::Clamp(
        FMath::IsFinite(Fraction) ? Fraction : 0.0f,
        0.0f,
        1.0f);
}

void UOmnisVehicleConditionComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (DeltaTime <= 0.0f || !FMath::IsFinite(DeltaTime) ||
        (GetOwner() && GetOwner()->GetNetMode() == NM_Client))
    {
        return;
    }

    const AActor* Owner = GetOwner();
    const float SpeedKph = Owner
        ? Owner->GetVelocity().Size() * 0.036f
        : 0.0f;
    const float Throttle = FMath::Abs(ThrottleInput);
    const float CargoFactor = 1.0f + FMath::Clamp(CargoLoadKg / 1000.0f, 0.0f, 1.5f);
    const float UphillLoadFactor = 1.0f +
        (FMath::Clamp(RoadGradePercent, 0.0f, 35.0f) / 100.0f) * 1.5f;
    const float Load = FMath::Clamp(
        EngineLoadFactor * CargoFactor * UphillLoadFactor,
        0.5f,
        3.5f);

    const float FuelRate = BaseFuelConsumptionLitersPerHour *
        (0.12f + (0.88f * Throttle * Throttle)) *
        Load;
    FuelLiters = FMath::Max(0.0f, FuelLiters - (FuelRate * DeltaTime / 3600.0f));

    const float CoolingFactor = FMath::Clamp(CoolingIntegrity / 100.0f, 0.0f, 1.0f);
    const float HeatInput = Throttle * Load * 0.55f;
    const float AirCooling = (0.004f + SpeedKph / 100000.0f) *
        FMath::Max(0.0f, EngineTemperatureC - AmbientTemperatureC) *
        CoolingFactor;
    EngineTemperatureC = FMath::Clamp(
        EngineTemperatureC + (HeatInput - AirCooling) * DeltaTime,
        AmbientTemperatureC,
        180.0f);

    const float DownhillBrakeLoad = FMath::Max(0.0f, -RoadGradePercent) / 100.0f;
    const float BrakeHeat = BrakeInput * SpeedKph *
        (1.55f + CargoFactor * 0.12f + DownhillBrakeLoad * 3.0f);
    const float BrakeCooling = (0.08f + SpeedKph * 0.003f) *
        FMath::Max(0.0f, BrakeTemperatureC - AmbientTemperatureC);
    BrakeTemperatureC = FMath::Clamp(
        BrakeTemperatureC + (BrakeHeat - BrakeCooling) * DeltaTime,
        AmbientTemperatureC,
        900.0f);

    if (EngineTemperatureC > 125.0f)
    {
        EngineIntegrity = FMath::Max(0.0f, EngineIntegrity - (EngineTemperatureC - 125.0f) * 0.0008f * DeltaTime);
        CoolingIntegrity = FMath::Max(0.0f, CoolingIntegrity - 0.002f * DeltaTime);
    }
    if (BrakeTemperatureC > 600.0f)
    {
        BrakeIntegrity = FMath::Max(0.0f, BrakeIntegrity - 0.004f * DeltaTime);
    }

    if (SaltExposure > 0.0f)
    {
        const float ExposureRate = SaltExposure * (Archetype == EOmnisVehicleArchetype::Boat ? 0.012f : 0.004f);
        CorrosionIntegrity = FMath::Max(0.0f, CorrosionIntegrity - ExposureRate * DeltaTime);
        TransmissionIntegrity = FMath::Max(0.0f, TransmissionIntegrity - ExposureRate * 0.2f * DeltaTime);
    }

    DistanceDrivenKm += SpeedKph * DeltaTime / 3600.0f;
    SteeringAlignmentDegrees = FMath::Clamp(
        SteeringAlignmentDegrees,
        -18.0f,
        18.0f);
}

void UOmnisVehicleConditionComponent::ApplyImpact(
    float ImpactSpeedKph,
    FVector ImpactDirection)
{
    if (GetOwner() && GetOwner()->GetNetMode() == NM_Client)
    {
        return;
    }

    const float Speed = FMath::Clamp(
        FMath::IsFinite(ImpactSpeedKph) ? ImpactSpeedKph : 0.0f,
        0.0f,
        240.0f);
    if (Speed < 18.0f)
    {
        return;
    }

    const float Severity = FMath::Clamp((Speed - 18.0f) / 90.0f, 0.0f, 1.5f);
    const FVector Direction = ImpactDirection.GetSafeNormal();
    const float LateralExposure = FMath::Abs(Direction.Y);
    const float VerticalExposure = FMath::Abs(Direction.Z);

    SuspensionIntegrity = FMath::Max(0.0f, SuspensionIntegrity - (Severity * (3.0f + LateralExposure * 8.0f)));
    TireIntegrity = FMath::Max(0.0f, TireIntegrity - (Severity * (1.5f + LateralExposure * 4.0f)));
    SteeringAlignmentDegrees = FMath::Clamp(
        SteeringAlignmentDegrees + FMath::Sign(Direction.Y) * Severity * 2.2f,
        -18.0f,
        18.0f);

    EngineIntegrity = FMath::Max(0.0f, EngineIntegrity - Severity * VerticalExposure * 2.0f);
    TransmissionIntegrity = FMath::Max(0.0f, TransmissionIntegrity - Severity * 1.5f);
}

void UOmnisVehicleConditionComponent::RepairComponent(
    FName ComponentId,
    float RepairAmount)
{
    if ((GetOwner() && GetOwner()->GetNetMode() == NM_Client) ||
        !FMath::IsFinite(RepairAmount) || RepairAmount <= 0.0f)
    {
        return;
    }

    const float Amount = FMath::Clamp(RepairAmount, 0.0f, 100.0f);
    if (ComponentId == TEXT("engine"))
    {
        EngineIntegrity = FMath::Clamp(EngineIntegrity + Amount, 0.0f, 100.0f);
    }
    else if (ComponentId == TEXT("transmission"))
    {
        TransmissionIntegrity = FMath::Clamp(TransmissionIntegrity + Amount, 0.0f, 100.0f);
    }
    else if (ComponentId == TEXT("suspension"))
    {
        SuspensionIntegrity = FMath::Clamp(SuspensionIntegrity + Amount, 0.0f, 100.0f);
    }
    else if (ComponentId == TEXT("brakes"))
    {
        BrakeIntegrity = FMath::Clamp(BrakeIntegrity + Amount, 0.0f, 100.0f);
    }
    else if (ComponentId == TEXT("tires"))
    {
        TireIntegrity = FMath::Clamp(TireIntegrity + Amount, 0.0f, 100.0f);
    }
    else if (ComponentId == TEXT("cooling"))
    {
        CoolingIntegrity = FMath::Clamp(CoolingIntegrity + Amount, 0.0f, 100.0f);
    }
    else if (ComponentId == TEXT("corrosion"))
    {
        CorrosionIntegrity = FMath::Clamp(CorrosionIntegrity + Amount, 0.0f, 100.0f);
    }
}

float UOmnisVehicleConditionComponent::GetPowertrainOutputScale() const
{
    if (FuelLiters <= 0.0f)
    {
        return 0.0f;
    }

    const float EngineFactor = FMath::Lerp(0.3f, 1.0f, EngineIntegrity / 100.0f);
    const float TransmissionFactor = FMath::Lerp(0.4f, 1.0f, TransmissionIntegrity / 100.0f);
    const float TemperatureFactor = EngineTemperatureC <= 105.0f
        ? 1.0f
        : FMath::GetMappedRangeValueClamped(
            FVector2D(105.0f, 150.0f),
            FVector2D(1.0f, 0.35f),
            EngineTemperatureC);
    return EngineFactor * TransmissionFactor * TemperatureFactor;
}

float UOmnisVehicleConditionComponent::GetBrakeOutputScale() const
{
    const float IntegrityFactor = FMath::Lerp(0.25f, 1.0f, BrakeIntegrity / 100.0f);
    const float TemperatureFactor = BrakeTemperatureC <= 420.0f
        ? 1.0f
        : FMath::GetMappedRangeValueClamped(
            FVector2D(420.0f, 700.0f),
            FVector2D(1.0f, 0.35f),
            BrakeTemperatureC);
    return IntegrityFactor * TemperatureFactor;
}

float UOmnisVehicleConditionComponent::GetTractionScale() const
{
    const float TireFactor = FMath::Lerp(0.3f, 1.0f, TireIntegrity / 100.0f);
    const float SuspensionFactor = FMath::Lerp(0.55f, 1.0f, SuspensionIntegrity / 100.0f);
    const float DriveFactor = DriveMode == EOmnisVehicleDriveMode::TwoWheelDrive
        ? 1.0f
        : 1.08f;
    return TireFactor * SuspensionFactor * SurfaceGripScale * DriveFactor;
}

float UOmnisVehicleConditionComponent::GetTorqueMultiplier() const
{
    return Archetype == EOmnisVehicleArchetype::FourByFour &&
        DriveMode == EOmnisVehicleDriveMode::FourWheelDriveLow
        ? 1.65f
        : 1.0f;
}

float UOmnisVehicleConditionComponent::GetBuoyancyLiftScale() const
{
    return IsBoat() ? ImmersionFraction : 0.0f;
}

FOmnisVehicleConditionSnapshot UOmnisVehicleConditionComponent::ExportSnapshot(
    FName PersistentId) const
{
    FOmnisVehicleConditionSnapshot Snapshot;
    Snapshot.PersistentId = PersistentId;
    Snapshot.Archetype = Archetype;
    Snapshot.DriveMode = DriveMode;
    Snapshot.FuelLiters = FuelLiters;
    Snapshot.EngineIntegrity = EngineIntegrity;
    Snapshot.TransmissionIntegrity = TransmissionIntegrity;
    Snapshot.SuspensionIntegrity = SuspensionIntegrity;
    Snapshot.BrakeIntegrity = BrakeIntegrity;
    Snapshot.TireIntegrity = TireIntegrity;
    Snapshot.CoolingIntegrity = CoolingIntegrity;
    Snapshot.CorrosionIntegrity = CorrosionIntegrity;
    Snapshot.EngineTemperatureC = EngineTemperatureC;
    Snapshot.BrakeTemperatureC = BrakeTemperatureC;
    Snapshot.SteeringAlignmentDegrees = SteeringAlignmentDegrees;
    Snapshot.DistanceDrivenKm = DistanceDrivenKm;
    return Snapshot;
}

void UOmnisVehicleConditionComponent::ImportSnapshot(
    const FOmnisVehicleConditionSnapshot& Snapshot)
{
    if (GetOwner() && GetOwner()->GetNetMode() == NM_Client)
    {
        return;
    }

    Archetype = Snapshot.Archetype;
    ApplyArchetypeDefaults();
    SetDriveMode(Snapshot.DriveMode);
    FuelLiters = FMath::Clamp(Snapshot.FuelLiters, 0.0f, FuelCapacityLiters);
    EngineIntegrity = FMath::Clamp(Snapshot.EngineIntegrity, 0.0f, 100.0f);
    TransmissionIntegrity = FMath::Clamp(Snapshot.TransmissionIntegrity, 0.0f, 100.0f);
    SuspensionIntegrity = FMath::Clamp(Snapshot.SuspensionIntegrity, 0.0f, 100.0f);
    BrakeIntegrity = FMath::Clamp(Snapshot.BrakeIntegrity, 0.0f, 100.0f);
    TireIntegrity = FMath::Clamp(Snapshot.TireIntegrity, 0.0f, 100.0f);
    CoolingIntegrity = FMath::Clamp(Snapshot.CoolingIntegrity, 0.0f, 100.0f);
    CorrosionIntegrity = FMath::Clamp(Snapshot.CorrosionIntegrity, 0.0f, 100.0f);
    EngineTemperatureC = FMath::Clamp(Snapshot.EngineTemperatureC, -20.0f, 180.0f);
    BrakeTemperatureC = FMath::Clamp(Snapshot.BrakeTemperatureC, -20.0f, 900.0f);
    SteeringAlignmentDegrees = FMath::Clamp(Snapshot.SteeringAlignmentDegrees, -18.0f, 18.0f);
    DistanceDrivenKm = FMath::Max(0.0f, Snapshot.DistanceDrivenKm);
}
