#include "Gameplay/Player/OmnisPlayerController.h"

#include "EngineUtils.h"
#include "Gameplay/Aviation/OmnisAircraftPawn.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Vehicle/OmnisVehiclePawn.h"

AOmnisPlayerController::AOmnisPlayerController()
{
    bShowMouseCursor = false;
}

void AOmnisPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameOnly());

    OnFootPawn = GetPawn();
}

void AOmnisPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);
    InputComponent->BindAction(TEXT("EnterVehicle"), IE_Pressed, this, &AOmnisPlayerController::ToggleNearestVehicle);
    InputComponent->BindAction(TEXT("EnterAircraft"), IE_Pressed, this, &AOmnisPlayerController::ToggleNearestAircraft);
}

APawn* AOmnisPlayerController::FindNearestPawn(UClass* PawnClass, float MaxDistance) const
{
    UWorld* World = GetWorld();
    if (!World || !PawnClass)
    {
        return nullptr;
    }

    const FVector Origin = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
    APawn* Best = nullptr;
    float BestDistanceSq = FMath::Square(MaxDistance);

    for (TActorIterator<APawn> It(World); It; ++It)
    {
        APawn* Candidate = *It;
        if (!Candidate || Candidate == GetPawn() || !Candidate->IsA(PawnClass))
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared(Origin, Candidate->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            Best = Candidate;
        }
    }

    return Best;
}

void AOmnisPlayerController::ToggleNearestVehicle()
{
    if (GetPawn() && GetPawn()->IsA(AOmnisVehiclePawn::StaticClass()))
    {
        if (OnFootPawn.IsValid())
        {
            Possess(OnFootPawn.Get());
        }
        return;
    }

    APawn* Vehicle = FindNearestPawn(AOmnisVehiclePawn::StaticClass(), 1600.0f);
    if (Vehicle)
    {
        OnFootPawn = GetPawn();
        Possess(Vehicle);
    }
}

void AOmnisPlayerController::ToggleNearestAircraft()
{
    if (GetPawn() && GetPawn()->IsA(AOmnisAircraftPawn::StaticClass()))
    {
        if (OnFootPawn.IsValid())
        {
            Possess(OnFootPawn.Get());
        }
        return;
    }

    APawn* Aircraft = FindNearestPawn(AOmnisAircraftPawn::StaticClass(), 3000.0f);
    if (Aircraft)
    {
        OnFootPawn = GetPawn();
        Possess(Aircraft);
    }
}
