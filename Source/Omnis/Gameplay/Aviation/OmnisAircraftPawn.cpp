#include "Gameplay/Aviation/OmnisAircraftPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AOmnisAircraftPawn::AOmnisAircraftPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    AircraftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AircraftMesh"));
    RootComponent = AircraftMesh;
    AircraftMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    AircraftMesh->SetSimulatePhysics(true);
    AircraftMesh->SetLinearDamping(0.12f);
    AircraftMesh->SetAngularDamping(0.65f);
}

void AOmnisAircraftPawn::ConfigureAircraft(EOmnisAircraftType InType, const FString& MeshAssetPath)
{
    AircraftType = InType;
    AircraftMeshPath = MeshAssetPath;
    ApplyConfiguredMesh();
}

void AOmnisAircraftPawn::ApplyConfiguredMesh()
{
    if (AircraftMeshPath.IsEmpty() || !AircraftMesh) return;

    if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *AircraftMeshPath))
    {
        AircraftMesh->SetStaticMesh(Mesh);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Omnis aircraft asset missing: %s"), *AircraftMeshPath);
    }
}

void AOmnisAircraftPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!AircraftMesh || !AircraftMesh->IsSimulatingPhysics()) return;

    const FVector Forward = GetActorForwardVector();
    const FVector Up = GetActorUpVector();

    AircraftMesh->AddForce(Forward * ThrustForce * FMath::Max(0.0f, ThrottleInput));

    const float LiftScale = AircraftType == EOmnisAircraftType::Helicopter
        ? (0.25f + FMath::Max(0.0f, ThrottleInput))
        : FMath::Clamp(ThrottleInput + 0.35f, 0.0f, 1.5f);

    AircraftMesh->AddForce(Up * LiftForce * LiftScale);

    AircraftMesh->AddTorqueInRadians(
        GetActorRightVector() * ControlTorque * PitchInput +
        Forward * ControlTorque * -RollInput +
        Up * ControlTorque * YawInput
    );
}

void AOmnisAircraftPawn::Throttle(float Value) { ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void AOmnisAircraftPawn::Pitch(float Value) { PitchInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void AOmnisAircraftPawn::Roll(float Value) { RollInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void AOmnisAircraftPawn::Yaw(float Value) { YawInput = FMath::Clamp(Value, -1.0f, 1.0f); }

void AOmnisAircraftPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("AircraftThrottle"), this, &AOmnisAircraftPawn::Throttle);
    PlayerInputComponent->BindAxis(TEXT("AircraftPitch"), this, &AOmnisAircraftPawn::Pitch);
    PlayerInputComponent->BindAxis(TEXT("AircraftRoll"), this, &AOmnisAircraftPawn::Roll);
    PlayerInputComponent->BindAxis(TEXT("AircraftYaw"), this, &AOmnisAircraftPawn::Yaw);
}
