#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Camera/Player/OmnisPlayerCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Gameplay/Exploration/OmnisExplorationComponent.h"
#include "Gameplay/Traversal/OmnisTraversalComponent.h"
#include "Gameplay/Interaction/OmnisInteractionComponent.h"
#include "Gameplay/Construction/OmnisConstructionComponent.h"
#include "Gameplay/Combat/OmnisCombatComponent.h"
#include "Gameplay/Abilities/OmnisAbilityComponent.h"
#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "Gameplay/Equipment/OmnisEquipmentComponent.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "Gameplay/Character/OmnisProceduralAnimationComponent.h"
#include "Gameplay/Player/OmnisGameMode.h"

AOmnisPlayerCharacter::AOmnisPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    GetCharacterMovement()->JumpZVelocity = 650.0f;
    GetCharacterMovement()->AirControl = 0.4f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 550.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UOmnisPlayerCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->bUsePawnControlRotation = false;

    UStaticMeshComponent* BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(GetCapsuleComponent());
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> AuthoredMesh(
        TEXT("/Game/Omnis/Art/Characters/Omnis_Wanderer.Omnis_Wanderer")
    );
    static ConstructorHelpers::FObjectFinder<UStaticMesh> FallbackMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube")
    );

    if (AuthoredMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(AuthoredMesh.Object);
    }
    else if (FallbackMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(FallbackMesh.Object);
    }

    BodyMesh->SetRelativeScale3D(
        AuthoredMesh.Succeeded() ? FVector(75.0f) : FVector(0.55f, 0.55f, 1.2f)
    );
    BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 12.0f));

    Exploration = CreateDefaultSubobject<UOmnisExplorationComponent>(TEXT("Exploration"));
    Traversal = CreateDefaultSubobject<UOmnisTraversalComponent>(TEXT("Traversal"));
    Interaction = CreateDefaultSubobject<UOmnisInteractionComponent>(TEXT("Interaction"));
    Construction = CreateDefaultSubobject<UOmnisConstructionComponent>(TEXT("Construction"));
    Combat = CreateDefaultSubobject<UOmnisCombatComponent>(TEXT("Combat"));
    Abilities = CreateDefaultSubobject<UOmnisAbilityComponent>(TEXT("Abilities"));
    Inventory = CreateDefaultSubobject<UOmnisInventoryComponent>(TEXT("Inventory"));
    Equipment = CreateDefaultSubobject<UOmnisEquipmentComponent>(TEXT("Equipment"));
    Health = CreateDefaultSubobject<UOmnisHealthComponent>(TEXT("Health"));
    ProceduralAnimation = CreateDefaultSubobject<UOmnisProceduralAnimationComponent>(TEXT("ProceduralAnimation"));
}

void AOmnisPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (FollowCamera)
    {
        FollowCamera->SetDistance(550.0f);
    }

    if (Health)
    {
        Health->OnDeath.AddDynamic(this, &AOmnisPlayerCharacter::HandlePlayerDeath);
    }
}

void AOmnisPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &AOmnisPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AOmnisPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AOmnisPlayerCharacter::SprintPressed);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AOmnisPlayerCharacter::SprintReleased);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AOmnisPlayerCharacter::AttackPressed);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AOmnisPlayerCharacter::InteractPressed);
    PlayerInputComponent->BindAction("Build", IE_Pressed, this, &AOmnisPlayerCharacter::BuildPressed);
}

void AOmnisPlayerCharacter::MoveForward(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void AOmnisPlayerCharacter::MoveRight(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void AOmnisPlayerCharacter::SprintPressed()
{
    GetCharacterMovement()->MaxWalkSpeed = 750.0f;
}

void AOmnisPlayerCharacter::SprintReleased()
{
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

void AOmnisPlayerCharacter::AttackPressed()
{
    if (Combat)
    {
        Combat->Attack();
    }
}

void AOmnisPlayerCharacter::InteractPressed()
{
    if (Interaction)
    {
        Interaction->Interact();
    }
}

void AOmnisPlayerCharacter::BuildPressed()
{
    if (Construction)
    {
        Construction->ConstructForward();
    }
}

void AOmnisPlayerCharacter::HandlePlayerDeath()
{
    if (AOmnisGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AOmnisGameMode>() : nullptr)
    {
        GameMode->NotifyPlayerDeath(GetController());
    }
}
