#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Camera/Player/OmnisPlayerCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Engine/LocalPlayer.h"
#include "InputCoreTypes.h"
#include "Gameplay/Exploration/OmnisExplorationComponent.h"
#include "Gameplay/Traversal/OmnisTraversalComponent.h"
#include "Gameplay/Interaction/OmnisInteractionComponent.h"
#include "Gameplay/Construction/OmnisConstructionComponent.h"
#include "Gameplay/Combat/OmnisCombatComponent.h"
#include "Gameplay/Abilities/OmnisAbilityComponent.h"
#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "Gameplay/Equipment/OmnisEquipmentComponent.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "Gameplay/Player/OmnisGameMode.h"
#include "UI/Menus/OmnisMenuStateSubsystem.h"

namespace
{
    UInputAction* MakeAction(UObject* Outer, const TCHAR* Name, EInputActionValueType ValueType)
    {
        UInputAction* Action = NewObject<UInputAction>(Outer, FName(Name));
        Action->ValueType = ValueType;
        return Action;
    }

    void AddKey(UInputMappingContext* Context, UInputAction* Action, const FKey& Key, bool bNegate)
    {
        FEnhancedActionKeyMapping& Mapping = Context->MapKey(Action, Key);
        if (bNegate)
        {
            Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(Context));
        }
    }
}

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

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(CubeMesh.Object);
    }

    BodyMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.2f));
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

void AOmnisPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RemoveEnhancedInput();

    if (Health)
    {
        Health->OnDeath.RemoveDynamic(this, &AOmnisPlayerCharacter::HandlePlayerDeath);
    }

    Super::EndPlay(EndPlayReason);
}

void AOmnisPlayerCharacter::InitializeEnhancedInput()
{
    if (bEnhancedInputRegistered || !InputMappingContext)
    {
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
        LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;

    if (!InputSubsystem)
    {
        return;
    }

    InputSubsystem->AddMappingContext(InputMappingContext, 0);
    bEnhancedInputRegistered = true;
}

void AOmnisPlayerCharacter::RemoveEnhancedInput()
{
    if (!bEnhancedInputRegistered)
    {
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
        LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;

    if (InputSubsystem && InputMappingContext)
    {
        InputSubsystem->RemoveMappingContext(InputMappingContext);
    }

    bEnhancedInputRegistered = false;
}

void AOmnisPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);

    InputMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_OmnisGameplay"));
    MoveForwardAction = MakeAction(this, TEXT("IA_MoveForward"), EInputActionValueType::Axis1D);
    MoveRightAction = MakeAction(this, TEXT("IA_MoveRight"), EInputActionValueType::Axis1D);
    LookYawAction = MakeAction(this, TEXT("IA_LookYaw"), EInputActionValueType::Axis1D);
    LookPitchAction = MakeAction(this, TEXT("IA_LookPitch"), EInputActionValueType::Axis1D);
    JumpAction = MakeAction(this, TEXT("IA_Jump"), EInputActionValueType::Boolean);
    SprintAction = MakeAction(this, TEXT("IA_Sprint"), EInputActionValueType::Boolean);
    AttackAction = MakeAction(this, TEXT("IA_Attack"), EInputActionValueType::Boolean);
    InteractAction = MakeAction(this, TEXT("IA_Interact"), EInputActionValueType::Boolean);
    BuildAction = MakeAction(this, TEXT("IA_Build"), EInputActionValueType::Boolean);
    MenuAction = MakeAction(this, TEXT("IA_Menu"), EInputActionValueType::Boolean);
    BackAction = MakeAction(this, TEXT("IA_Back"), EInputActionValueType::Boolean);

    AddKey(InputMappingContext, MoveForwardAction, EKeys::W, false);
    AddKey(InputMappingContext, MoveForwardAction, EKeys::S, true);
    AddKey(InputMappingContext, MoveForwardAction, EKeys::Gamepad_LeftY, false);
    AddKey(InputMappingContext, MoveRightAction, EKeys::D, false);
    AddKey(InputMappingContext, MoveRightAction, EKeys::A, true);
    AddKey(InputMappingContext, MoveRightAction, EKeys::Gamepad_LeftX, false);
    AddKey(InputMappingContext, LookYawAction, EKeys::MouseX, false);
    AddKey(InputMappingContext, LookYawAction, EKeys::Gamepad_RightX, false);
    AddKey(InputMappingContext, LookPitchAction, EKeys::MouseY, true);
    AddKey(InputMappingContext, LookPitchAction, EKeys::Gamepad_RightY, true);
    AddKey(InputMappingContext, JumpAction, EKeys::SpaceBar, false);
    AddKey(InputMappingContext, JumpAction, EKeys::Gamepad_FaceButton_Bottom, false);
    AddKey(InputMappingContext, SprintAction, EKeys::LeftShift, false);
    AddKey(InputMappingContext, SprintAction, EKeys::Gamepad_LeftThumbstick, false);
    AddKey(InputMappingContext, AttackAction, EKeys::LeftMouseButton, false);
    AddKey(InputMappingContext, AttackAction, EKeys::Gamepad_RightTrigger, false);
    AddKey(InputMappingContext, InteractAction, EKeys::E, false);
    AddKey(InputMappingContext, InteractAction, EKeys::Gamepad_FaceButton_Right, false);
    AddKey(InputMappingContext, BuildAction, EKeys::Q, false);
    AddKey(InputMappingContext, BuildAction, EKeys::Gamepad_FaceButton_Left, false);
    AddKey(InputMappingContext, MenuAction, EKeys::Tab, false);
    AddKey(InputMappingContext, MenuAction, EKeys::Gamepad_Special_Right, false);
    AddKey(InputMappingContext, BackAction, EKeys::Escape, false);
    AddKey(InputMappingContext, BackAction, EKeys::Gamepad_Special_Left, false);

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInput)
    {
        return;
    }

    EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ThisClass::MoveForwardInput);
    EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ThisClass::MoveRightInput);
    EnhancedInput->BindAction(LookYawAction, ETriggerEvent::Triggered, this, &ThisClass::LookYawInput);
    EnhancedInput->BindAction(LookPitchAction, ETriggerEvent::Triggered, this, &ThisClass::LookPitchInput);
    EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::JumpInput);
    EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::JumpInput);
    EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::SprintInput);
    EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::SprintInput);
    EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AttackInput);
    EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::InteractInput);
    EnhancedInput->BindAction(BuildAction, ETriggerEvent::Started, this, &ThisClass::BuildInput);
    EnhancedInput->BindAction(MenuAction, ETriggerEvent::Started, this, &ThisClass::MenuInput);
    EnhancedInput->BindAction(BackAction, ETriggerEvent::Started, this, &ThisClass::BackInput);

    InitializeEnhancedInput();
}

void AOmnisPlayerCharacter::MoveForwardInput(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    if (Controller && FMath::Abs(Axis) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(GetActorForwardVector(), Axis);
    }
}

void AOmnisPlayerCharacter::MoveRightInput(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    if (Controller && FMath::Abs(Axis) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(GetActorRightVector(), Axis);
    }
}

void AOmnisPlayerCharacter::LookYawInput(const FInputActionValue& Value)
{
    AddControllerYawInput(Value.Get<float>());
}

void AOmnisPlayerCharacter::LookPitchInput(const FInputActionValue& Value)
{
    AddControllerPitchInput(Value.Get<float>());
}

void AOmnisPlayerCharacter::JumpInput(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        Jump();
    }
    else
    {
        StopJumping();
    }
}

void AOmnisPlayerCharacter::SprintInput(const FInputActionValue& Value)
{
    GetCharacterMovement()->MaxWalkSpeed = Value.Get<bool>() ? 750.0f : 500.0f;
}

void AOmnisPlayerCharacter::AttackInput(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && Combat)
    {
        Combat->Attack();
    }
}

void AOmnisPlayerCharacter::InteractInput(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && Interaction)
    {
        Interaction->Interact();
    }
}

void AOmnisPlayerCharacter::BuildInput(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && Construction)
    {
        Construction->ConstructForward();
    }
}

void AOmnisPlayerCharacter::MenuInput(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())
    {
        return;
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisMenuStateSubsystem* Menu = GameInstance->GetSubsystem<UOmnisMenuStateSubsystem>())
        {
            if (Menu->GetOpenMenu().IsNone())
            {
                Menu->OpenMenu(TEXT("Main"));
            }
            else
            {
                Menu->CloseMenu();
            }
        }
    }
}

void AOmnisPlayerCharacter::BackInput(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())
    {
        return;
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisMenuStateSubsystem* Menu = GameInstance->GetSubsystem<UOmnisMenuStateSubsystem>())
        {
            Menu->CloseMenu();
        }
    }
}

void AOmnisPlayerCharacter::HandlePlayerDeath()
{
    if (AOmnisGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AOmnisGameMode>() : nullptr)
    {
        GameMode->NotifyPlayerDeath(GetController());
    }
}
