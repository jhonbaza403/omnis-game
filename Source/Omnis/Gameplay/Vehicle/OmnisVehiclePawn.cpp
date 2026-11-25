#include "Gameplay/Vehicle/OmnisVehiclePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "World/Terrain/OmnisVenezuelaTerrainSubsystem.h"

AOmnisVehiclePawn::AOmnisVehiclePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
    RootComponent = VehicleMesh;
    VehicleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    VehicleMesh->SetSimulatePhysics(true);
    VehicleMesh->SetLinearDamping(0.35f);
    VehicleMesh->SetAngularDamping(1.4f);
    VehicleMesh->SetNotifyRigidBodyCollision(true);
    VehicleMesh->OnComponentHit.AddDynamic(this, &AOmnisVehiclePawn::HandleVehicleImpact);
    VehicleCondition = CreateDefaultSubobject<UOmnisVehicleConditionComponent>(TEXT("VehicleCondition"));
}

void AOmnisVehiclePawn::ConfigureVehicle(const FString& MeshAssetPath)
{
    VehicleMeshPath = MeshAssetPath;
    ApplyConfiguredMesh();
}

void AOmnisVehiclePawn::ConfigureVehicleArchetype(EOmnisVehicleArchetype NewArchetype)
{
    if (VehicleCondition)
    {
        VehicleCondition->SetArchetype(NewArchetype);
    }
}

void AOmnisVehiclePawn::SetTerrainContext(
    FName CellId,
    FName SurfaceId,
    float RainIntensity,
    float SaltExposure,
    float AmbientTemperature)
{
    TerrainCellId = CellId;
    TerrainSurfaceId = SurfaceId;
    TerrainRainIntensity = FMath::Clamp(RainIntensity, 0.0f, 1.0f);
    TerrainSaltExposure = FMath::Clamp(SaltExposure, 0.0f, 1.0f);
    AmbientTemperatureC = FMath::Clamp(AmbientTemperature, -20.0f, 55.0f);
}

void AOmnisVehiclePawn::SetWaterImmersion(float ImmersionFraction)
{
    if (VehicleCondition)
    {
        VehicleCondition->SetImmersionFraction(ImmersionFraction);
    }
}

void AOmnisVehiclePawn::ApplyConfiguredMesh()
{
    if (VehicleMeshPath.IsEmpty() || !VehicleMesh) return;

    if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *VehicleMeshPath))
    {
        VehicleMesh->SetStaticMesh(Mesh);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Omnis vehicle asset missing: %s"), *VehicleMeshPath);
    }
}

void AOmnisVehiclePawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!VehicleMesh || !VehicleMesh->IsSimulatingPhysics())
    {
        return;
    }

    const FVector Forward = GetActorForwardVector();
    const FVector Up = GetActorUpVector();
    const float BrakeInputValue = bBrake ? 1.0f : 0.0f;

    if (VehicleCondition)
    {
        VehicleCondition->SetDriverInputs(ThrottleInput, BrakeInputValue);
        VehicleCondition->SetAmbientConditions(AmbientTemperatureC, TerrainSaltExposure);
    }

    if (UOmnisVenezuelaTerrainSubsystem* Terrain =
        GetWorld() ? GetWorld()->GetSubsystem<UOmnisVenezuelaTerrainSubsystem>() : nullptr)
    {
        FOmnisTerrainSurfaceResponse SurfaceResponse;
        if (!TerrainSurfaceId.IsNone() &&
            Terrain->GetSurfaceResponse(TerrainSurfaceId, TerrainRainIntensity, TerrainSaltExposure, SurfaceResponse))
        {
            if (VehicleCondition)
            {
                VehicleCondition->SetSurfaceGripScale(SurfaceResponse.GripMultiplier);
            }
            if (!TerrainCellId.IsNone())
            {
                const float WheelLoadKg = VehicleMesh->GetMass() * 0.25f;
                const float SpeedKph = VehicleMesh->GetPhysicsLinearVelocity().Size() * 0.036f;
                Terrain->RecordWheelPass(TerrainCellId, TerrainSurfaceId, WheelLoadKg, SpeedKph, TerrainRainIntensity, DeltaSeconds);
            }
        }
    }

    if (FMath::Abs(ThrottleInput) > KINDA_SMALL_NUMBER)
    {
        const float PowertrainScale = VehicleCondition ? VehicleCondition->GetPowertrainOutputScale() : 1.0f;
        const float TractionScale = VehicleCondition ? VehicleCondition->GetTractionScale() : 1.0f;
        const float TorqueScale = VehicleCondition ? VehicleCondition->GetTorqueMultiplier() : 1.0f;
        VehicleMesh->AddForce(Forward * EngineForce * ThrottleInput * PowertrainScale * TractionScale * TorqueScale);
    }

    if (FMath::Abs(SteerInput) > KINDA_SMALL_NUMBER)
    {
        const float AlignmentBias = VehicleCondition ? VehicleCondition->GetSteeringBiasDegrees() : 0.0f;
        VehicleMesh->AddTorqueInRadians(
            Up * SteeringTorque * (SteerInput - FMath::Clamp(AlignmentBias / 30.0f, -0.6f, 0.6f)));
    }

    if (bBrake)
    {
        const FVector Velocity = VehicleMesh->GetPhysicsLinearVelocity();
        if (!Velocity.IsNearlyZero())
        {
            const float BrakeScale = VehicleCondition ? VehicleCondition->GetBrakeOutputScale() : 1.0f;
            VehicleMesh->AddForce(-Velocity.GetSafeNormal() * BrakeForce * BrakeScale);
        }
    }

    if (VehicleCondition && VehicleCondition->IsBoat())
    {
        const float Immersion = VehicleCondition->GetBuoyancyLiftScale();
        if (Immersion > 0.0f)
        {
            VehicleMesh->AddForce(Up * VehicleMesh->GetMass() * 980.0f * Immersion);
        }
    }
}

void AOmnisVehiclePawn::Throttle(float Value)
{
    ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AOmnisVehiclePawn::Steer(float Value)
{
    SteerInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AOmnisVehiclePawn::BrakePressed() { bBrake = true; }
void AOmnisVehiclePawn::BrakeReleased() { bBrake = false; }

void AOmnisVehiclePawn::HandleVehicleImpact(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!VehicleCondition || !VehicleMesh || GetNetMode() == NM_Client)
    {
        return;
    }
    const float ImpactSpeedKph = VehicleMesh->GetPhysicsLinearVelocity().Size() * 0.036f;
    VehicleCondition->ApplyImpact(ImpactSpeedKph, Hit.ImpactNormal);
}

FOmnisVehicleConditionSnapshot AOmnisVehiclePawn::ExportVehicleCondition() const
{
    return VehicleCondition
        ? VehicleCondition->ExportSnapshot(VehiclePersistentId)
        : FOmnisVehicleConditionSnapshot();
}

void AOmnisVehiclePawn::ImportVehicleCondition(const FOmnisVehicleConditionSnapshot& Snapshot)
{
    if (VehicleCondition)
    {
        VehicleCondition->ImportSnapshot(Snapshot);
    }
}

void AOmnisVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("VehicleThrottle"), this, &AOmnisVehiclePawn::Throttle);
    PlayerInputComponent->BindAxis(TEXT("VehicleSteer"), this, &AOmnisVehiclePawn::Steer);
    PlayerInputComponent->BindAction(TEXT("VehicleBrake"), IE_Pressed, this, &AOmnisVehiclePawn::BrakePressed);
    PlayerInputComponent->BindAction(TEXT("VehicleBrake"), IE_Released, this, &AOmnisVehiclePawn::BrakeReleased);
}
