#include "AI/NPC/OmnisNPCCharacter.h"

#include "AIController.h"
#include "AI/Perception/OmnisPerceptionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Time/OmnisTimeSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Character/OmnisProceduralAnimationComponent.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Data/Characters/OmnisCharacterDefinition.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

AOmnisNPCCharacter::AOmnisNPCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f;
    SetReplicates(true);
    SetReplicateMovement(true);
    NetUpdateFrequency = 2.0f;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();

    GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);
    GetCharacterMovement()->MaxWalkSpeed = 180.0f;
    InteractionPrompt = FText::FromString(TEXT("Hablar"));

    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    Visual->SetupAttachment(GetCapsuleComponent());
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> AuthoredMesh(
        TEXT("/Game/Omnis/Art/Characters/Omnis_Wanderer.Omnis_Wanderer"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> FallbackMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube"));

    if (AuthoredMesh.Succeeded())
    {
        Visual->SetStaticMesh(AuthoredMesh.Object);
    }
    else if (FallbackMesh.Succeeded())
    {
        Visual->SetStaticMesh(FallbackMesh.Object);
    }

    Visual->SetRelativeScale3D(
        AuthoredMesh.Succeeded() ? FVector(45.0f) : FVector(0.45f, 0.45f, 1.05f));
    Visual->SetRelativeLocation(FVector(0.0f, 0.0f, 8.0f));

    Health = CreateDefaultSubobject<UOmnisHealthComponent>(TEXT("Health"));
    Health->bDestroyOwnerOnDeath = false;
    Health->OnDeath.AddDynamic(this, &AOmnisNPCCharacter::HandleNPCDeath);

    CharacterState = CreateDefaultSubobject<UOmnisCharacterStateComponent>(TEXT("CharacterState"));
    Perception = CreateDefaultSubobject<UOmnisPerceptionComponent>(TEXT("Perception"));
    ProceduralAnimation = CreateDefaultSubobject<UOmnisProceduralAnimationComponent>(TEXT("ProceduralAnimation"));

    Tags.Add(TEXT("Omnis.NPC"));
    Tags.Add(TEXT("Omnis.Discoverable"));
}

void AOmnisNPCCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        ApplyCharacterDefinition();
    }

    HomeLocation = GetActorLocation();
    WorkLocation = HomeLocation + WorkLocationOffset;
    CurrentDestination = HomeLocation;
    bHasLifeDestination = true;

    if (CharacterState)
    {
        CharacterState->SetAlive(true);
        CharacterState->RecordMemory(TEXT("NPC.Spawned"), CharacterId, 10);
    }
}

void AOmnisNPCCharacter::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AOmnisNPCCharacter, CharacterId);
    DOREPLIFETIME(AOmnisNPCCharacter, PersistentId);
    DOREPLIFETIME(AOmnisNPCCharacter, CharacterDisplayName);
    DOREPLIFETIME(AOmnisNPCCharacter, CharacterBiography);
    DOREPLIFETIME(AOmnisNPCCharacter, OccupationId);
    DOREPLIFETIME(AOmnisNPCCharacter, HomeLocationId);
    DOREPLIFETIME(AOmnisNPCCharacter, WorkLocationId);
}

void AOmnisNPCCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // The server owns schedules, needs and navigation; clients only consume replicated state.
    if (!HasAuthority())
    {
        return;
    }

    if (!CharacterState || !CharacterState->IsAlive())
    {
        return;
    }

    LifeUpdateClock += DeltaSeconds;
    const float UpdateEvery = FMath::Max(1.0f, LifeUpdateInterval);
    if (LifeUpdateClock < UpdateEvery)
    {
        return;
    }

    const float Elapsed = LifeUpdateClock;
    LifeUpdateClock = 0.0f;
    SimulateLife(Elapsed);
}

void AOmnisNPCCharacter::ApplyCharacterDefinition()
{
    UOmnisCharacterDefinition* Definition = CharacterDefinition.LoadSynchronous();
    if (!Definition)
    {
        return;
    }

    if (!Definition->CharacterId.IsNone())
    {
        CharacterId = Definition->CharacterId;
    }
    CharacterDisplayName = Definition->DisplayName;
    CharacterBiography = Definition->Biography;
    OccupationId = Definition->OccupationId;
    HomeLocationId = Definition->HomeLocationId;
    WorkLocationId = Definition->WorkLocationId;
    WorkLocationOffset = Definition->WorkLocationOffset;
    LifeUpdateInterval = FMath::Clamp(Definition->LifeUpdateInterval, 1.0f, 20.0f);
    WorkStartHour = FMath::Fmod(FMath::Max(0.0f, Definition->WorkStartHour), 24.0f);
    WorkEndHour = FMath::Fmod(FMath::Max(0.0f, Definition->WorkEndHour), 24.0f);
    RestStartHour = FMath::Fmod(FMath::Max(0.0f, Definition->RestStartHour), 24.0f);
    RestEndHour = FMath::Fmod(FMath::Max(0.0f, Definition->RestEndHour), 24.0f);
    HungerThreshold = FMath::Clamp(Definition->HungerThreshold, 0.0f, 100.0f);
    EnergyThreshold = FMath::Clamp(Definition->EnergyThreshold, 0.0f, 100.0f);
    SocialThreshold = FMath::Clamp(Definition->SocialThreshold, 0.0f, 100.0f);
    ExploreRadius = FMath::Clamp(Definition->ExploreRadius, 100.0f, 5000.0f);
    SocialRadius = FMath::Clamp(Definition->SocialRadius, 100.0f, 5000.0f);

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(Definition->WalkSpeed, 50.0f, 800.0f);
    }
    if (Health)
    {
        Health->SetMaxHealth(FMath::Max(1.0f, Definition->MaxHealth));
    }
    if (UStaticMesh* DefinitionMesh = Definition->VisualMesh.LoadSynchronous())
    {
        if (Visual)
        {
            Visual->SetStaticMesh(DefinitionMesh);
        }
    }
}

void AOmnisNPCCharacter::SimulateLife(float DeltaSeconds)
{
    if (!CharacterState || !CharacterState->IsAlive())
    {
        return;
    }

    float GameHour = 12.0f;
    if (UWorld* World = GetWorld())
    {
        if (UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>())
        {
            // One in-game hour equals sixty simulated seconds at time scale 1.
            GameHour = FMath::Fmod(Time->GetElapsedSeconds() / 60.0f, 24.0f);
        }
    }

    const EOmnisCharacterLifeGoal PreviousGoal = CharacterState->GetCurrentGoal();
    EOmnisCharacterLifeGoal Goal = EOmnisCharacterLifeGoal::Explore;

    if (CharacterState->GetHunger() >= FMath::Clamp(HungerThreshold, 0.0f, 100.0f))
    {
        Goal = EOmnisCharacterLifeGoal::Eat;
    }
    else if (CharacterState->GetEnergy() <= FMath::Clamp(EnergyThreshold, 0.0f, 100.0f))
    {
        Goal = EOmnisCharacterLifeGoal::Rest;
    }
    else if (CharacterState->GetSocialNeed() >= FMath::Clamp(SocialThreshold, 0.0f, 100.0f))
    {
        Goal = EOmnisCharacterLifeGoal::Socialize;
    }
    else
    {
        const auto IsWithinHourWindow = [](float Hour, float StartHour, float EndHour)
        {
            return StartHour <= EndHour
                ? Hour >= StartHour && Hour < EndHour
                : Hour >= StartHour || Hour < EndHour;
        };

        if (IsWithinHourWindow(GameHour, WorkStartHour, WorkEndHour))
        {
            Goal = EOmnisCharacterLifeGoal::Work;
        }
        else if (IsWithinHourWindow(GameHour, RestStartHour, RestEndHour))
        {
            Goal = EOmnisCharacterLifeGoal::Rest;
        }
    }

    AActor* SocialTarget = nullptr;
    if (Goal == EOmnisCharacterLifeGoal::Socialize && Perception)
    {
        SocialTarget = Perception->SenseClosest(FMath::Clamp(SocialRadius, 100.0f, 5000.0f));
        if (SocialTarget &&
            !Cast<AOmnisPlayerCharacter>(SocialTarget) &&
            !Cast<AOmnisNPCCharacter>(SocialTarget))
        {
            SocialTarget = nullptr;
        }
        if (!SocialTarget)
        {
            Goal = EOmnisCharacterLifeGoal::Explore;
        }
    }

    CharacterState->SetCurrentGoal(Goal);
    const bool bGoalChanged = PreviousGoal != Goal;
    bool bNewExploreDestination = false;

    FVector Destination = HomeLocation;
    switch (Goal)
    {
        case EOmnisCharacterLifeGoal::Work:
            Destination = WorkLocation;
            break;

        case EOmnisCharacterLifeGoal::Eat:
        case EOmnisCharacterLifeGoal::Rest:
            Destination = HomeLocation;
            break;

        case EOmnisCharacterLifeGoal::Socialize:
            if (SocialTarget)
            {
                Destination = SocialTarget->GetActorLocation();
            }
            break;

        case EOmnisCharacterLifeGoal::Explore:
            if (bGoalChanged ||
                FVector::DistSquared(GetActorLocation(), CurrentDestination) < FMath::Square(150.0f))
            {
                CurrentDestination = HomeLocation + FVector(
                    FMath::FRandRange(-FMath::Clamp(ExploreRadius, 100.0f, 5000.0f), FMath::Clamp(ExploreRadius, 100.0f, 5000.0f)),
                    FMath::FRandRange(-FMath::Clamp(ExploreRadius, 100.0f, 5000.0f), FMath::Clamp(ExploreRadius, 100.0f, 5000.0f)),
                    0.0f);
                bNewExploreDestination = true;
            }
            Destination = CurrentDestination;
            break;

        case EOmnisCharacterLifeGoal::Idle:
        case EOmnisCharacterLifeGoal::SeekSafety:
        default:
            Destination = HomeLocation;
            break;
    }

    const bool bDestinationChanged =
        bNewExploreDestination ||
        !bHasLifeDestination ||
        FVector::DistSquared(Destination, CurrentDestination) > FMath::Square(200.0f);

    if (bGoalChanged || bDestinationChanged)
    {
        CurrentDestination = Destination;
        bHasLifeDestination = true;
        MoveToLifeDestination(
            CurrentDestination,
            Goal == EOmnisCharacterLifeGoal::Socialize ? 180.0f : 100.0f);
    }

    const bool bReachedGoal =
        FVector::DistSquared(GetActorLocation(), Destination) <= FMath::Square(180.0f);

    if (Goal == EOmnisCharacterLifeGoal::Socialize && SocialTarget && bReachedGoal)
    {
        FName SocialTargetId = SocialTarget->GetFName();
        if (Cast<AOmnisPlayerCharacter>(SocialTarget))
        {
            SocialTargetId = TEXT("Player");
        }
        else if (AOmnisNPCCharacter* OtherNPC = Cast<AOmnisNPCCharacter>(SocialTarget))
        {
            SocialTargetId = OtherNPC->GetPersistentId();
            if (SocialTargetId.IsNone())
            {
                SocialTargetId = OtherNPC->CharacterId;
            }
        }
        CharacterState->RecordMemory(TEXT("NPC.SocialEncounter"), SocialTargetId, 5);
        CharacterState->AdjustRelationship(SocialTargetId, 1);

        if (AOmnisNPCCharacter* OtherNPC = Cast<AOmnisNPCCharacter>(SocialTarget))
        {
            if (OtherNPC->CharacterState)
            {
                OtherNPC->CharacterState->RecordMemory(
                    TEXT("NPC.SocialEncounter"),
                    GetPersistentId(),
                    5);
                OtherNPC->CharacterState->AdjustRelationship(GetPersistentId(), 1);
            }
        }
        else if (AOmnisPlayerCharacter* Player = Cast<AOmnisPlayerCharacter>(SocialTarget))
        {
            if (Player->CharacterState)
            {
                Player->CharacterState->RecordMemory(
                    TEXT("Player.SocialEncounter"),
                    GetPersistentId(),
                    5);
            }
        }
    }

    CharacterState->SimulateNeeds(DeltaSeconds, Goal, bReachedGoal);
}

void AOmnisNPCCharacter::MoveToLifeDestination(
    const FVector& Destination,
    float AcceptanceRadius)
{
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->MoveToLocation(Destination, AcceptanceRadius);
    }
}

FText AOmnisNPCCharacter::GetInteractionPrompt_Implementation() const
{
    if (CharacterState && CharacterState->HasMemory(TEXT("Mission.Failure.PublicShame")))
    {
        return NSLOCTEXT("Omnis", "PublicShamePrompt", "No olvido lo que ocurrió");
    }

    if (!CharacterDisplayName.IsEmpty())
    {
        return FText::Format(
            NSLOCTEXT("Omnis", "TalkToNamedCharacter", "Hablar con {0}"),
            CharacterDisplayName);
    }

    return InteractionPrompt;
}

void AOmnisNPCCharacter::Interact_Implementation(AActor* Interactor)
{
    if (!HasAuthority() || !Interactor || !CharacterState || !CharacterState->IsAlive())
    {
        return;
    }

    const AOmnisPlayerCharacter* PlayerInteractor = Cast<AOmnisPlayerCharacter>(Interactor);
    const FName InteractorId = PlayerInteractor ? FName(TEXT("Player")) : Interactor->GetFName();
    const int32 Relationship = CharacterState->GetRelationship(InteractorId);

    // Witnesses who have repeatedly lost trust can refuse future interaction.
    if (Relationship <= -50)
    {
        CharacterState->RecordMemory(TEXT("NPC.RefusedPlayer"), InteractorId, 35);
        return;
    }

    CharacterState->RecordMemory(TEXT("NPC.Interaction"), InteractorId, 25);
    CharacterState->AdjustRelationship(InteractorId, 5);

    if (AOmnisPlayerCharacter* Player = Cast<AOmnisPlayerCharacter>(Interactor))
    {
        if (Player->CharacterState)
        {
            Player->CharacterState->RecordMemory(
                TEXT("Player.InteractedWithNPC"),
                CharacterId,
                15);
        }
    }
}

void AOmnisNPCCharacter::HandleNPCDeath()
{
    if (CharacterState)
    {
        CharacterState->SetAlive(false);
        CharacterState->RecordMemory(TEXT("NPC.Death"), CharacterId, 100);
    }

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
    }

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->DisableMovement();
    }

    Tags.Add(TEXT("Omnis.Dead"));
}

FName AOmnisNPCCharacter::GetPersistentId() const
{
    return PersistentId;
}

void AOmnisNPCCharacter::RestorePersistentState(
    const FTransform& SavedTransform,
    const FOmnisCharacterLifeSnapshot& SavedLifeState)
{
    if (!HasAuthority())
    {
        return;
    }

    SetActorTransform(
        SavedTransform,
        false,
        nullptr,
        ETeleportType::TeleportPhysics);

    if (CharacterState)
    {
        CharacterState->ImportSnapshot(SavedLifeState);
    }

    if (Health)
    {
        Health->SetDeadState(!SavedLifeState.bAlive);
    }

    if (!SavedLifeState.bAlive)
    {
        Tags.AddUnique(TEXT("Omnis.Dead"));
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            AIController->StopMovement();
        }
        if (GetCharacterMovement())
        {
            GetCharacterMovement()->DisableMovement();
        }
    }
    else
    {
        Tags.Remove(TEXT("Omnis.Dead"));
        if (GetCharacterMovement())
        {
            GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }
        CurrentDestination = GetActorLocation();
        bHasLifeDestination = true;
    }
}
