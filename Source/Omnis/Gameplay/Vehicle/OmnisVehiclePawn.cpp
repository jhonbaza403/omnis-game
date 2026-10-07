#include "Gameplay/Vehicle/OmnisVehiclePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AOmnisVehiclePawn::AOmnisVehiclePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
    RootComponent = VehicleMesh;
    VehicleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    VehicleMesh->SetSimulatePhysics(true);
    VehicleMesh->SetLinearDamping(0.35f);
    VehicleMesh->SetAngularDamping(1.4f);
}

void AOmnisVehiclePawn::ConfigureVehicle(const FString& MeshAssetPath)
{
    VehicleMeshPath = MeshAssetPath;
    ApplyConfiguredMesh();
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
    if (!VehicleMesh || !VehicleMesh->IsSimulatingPhysics()) return;

    const FVector Forward = GetActorForwardVector();
    const FVector Up = GetActorUpVector();

    if (FMath::Abs(ThrottleInput) > KINDA_SMALL_NUMBER)
    {
        VehicleMesh->AddForce(Forward * EngineForce * ThrottleInput);
    }

    if (FMath::Abs(SteerInput) > KINDA_SMALL_NUMBER)
    {
        VehicleMesh->AddTorqueInRadians(Up * SteeringTorque * SteerInput);
    }

    if (bBrake)
    {
        const FVector Velocity = VehicleMesh->GetPhysicsLinearVelocity();
        if (!Velocity.IsNearlyZero())
        {
            VehicleMesh->AddForce(-Velocity.GetSafeNormal() * BrakeForce);
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

void AOmnisVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("VehicleThrottle"), this, &AOmnisVehiclePawn::Throttle);
    PlayerInputComponent->BindAxis(TEXT("VehicleSteer"), this, &AOmnisVehiclePawn::Steer);
    PlayerInputComponent->BindAction(TEXT("VehicleBrake"), IE_Pressed, this, &AOmnisVehiclePawn::BrakePressed);
    PlayerInputComponent->BindAction(TEXT("VehicleBrake"), IE_Released, this, &AOmnisVehiclePawn::BrakeReleased);
}
