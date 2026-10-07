#include "AI/NPC/OmnisNPCCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "Gameplay/Character/OmnisProceduralAnimationComponent.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"

AOmnisNPCCharacter::AOmnisNPCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);
    GetCharacterMovement()->MaxWalkSpeed = 180.0f;
    InteractionPrompt = FText::FromString(TEXT("Hablar"));

    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    Visual->SetupAttachment(GetCapsuleComponent());
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> AuthoredMesh(
        TEXT("/Game/Omnis/Art/Characters/Omnis_Wanderer.Omnis_Wanderer")
    );
    static ConstructorHelpers::FObjectFinder<UStaticMesh> FallbackMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube")
    );

    if (AuthoredMesh.Succeeded())
    {
        Visual->SetStaticMesh(AuthoredMesh.Object);
    }
    else if (FallbackMesh.Succeeded())
    {
        Visual->SetStaticMesh(FallbackMesh.Object);
    }

    Visual->SetRelativeScale3D(
        AuthoredMesh.Succeeded() ? FVector(45.0f) : FVector(0.45f, 0.45f, 1.05f)
    );
    Visual->SetRelativeLocation(FVector(0.0f, 0.0f, 8.0f));

    Health = CreateDefaultSubobject<UOmnisHealthComponent>(TEXT("Health"));
    Tags.Add(TEXT("Omnis.NPC"));
    Tags.Add(TEXT("Omnis.Discoverable"));
    ProceduralAnimation = CreateDefaultSubobject<UOmnisProceduralAnimationComponent>(TEXT("ProceduralAnimation"));
}

void AOmnisNPCCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    RoutineClock += DeltaSeconds;
    if (FMath::Fmod(RoutineClock, 4.0f) < DeltaSeconds)
    {
        RoutineOrigin = GetActorLocation();
    }

    const float Direction = FMath::Sin(RoutineClock * 0.5f);
    AddMovementInput(FVector(Direction, 0.15f * FMath::Cos(RoutineClock), 0.0f), 0.05f);
}

void AOmnisNPCCharacter::Interact_Implementation(AActor* Interactor)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisProgressionSubsystem* Progression = GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
        {
            Progression->AddExperience(10);
        }
    }
}
