#include "AI/NPC/OmnisNPCCharacter.h"

#include "AIController.h"
#include "AI/NPC/OmnisCharacterLifeSubsystem.h"
#include "AI/NPC/OmnisNPCDialogueSubsystem.h"
#include "Engine/GameInstance.h"
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
    PrimaryActorTick.bCanEverTick = false;
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

    if (UOmnisCharacterLifeSubsystem* Life = GetWorld()
        ? GetWorld()->GetSubsystem<UOmnisCharacterLifeSubsystem>()
        : nullptr)
    {
        HomeLocation = Life->ResolveLocation(HomeLocationId, HomeLocation);
        WorkLocation = Life->ResolveLocation(WorkLocationId, WorkLocation);
    }

    CurrentDestination = HomeLocation;
    bHasLifeDestination = true;

    if (CharacterState)
    {
        CharacterState->SetAlive(true);
        CharacterState->RecordMemory(TEXT("NPC.Spawned"), CharacterId, 10);
    }

    if (UOmnisCharacterLifeSubsystem* Life = GetWorld()
        ? GetWorld()->GetSubsystem<UOmnisCharacterLifeSubsystem>()
        : nullptr)
    {
        Life->RegisterNPC(this);
    }
}

void AOmnisNPCCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UOmnisCharacterLifeSubsystem* Life = GetWorld()
        ? GetWorld()->GetSubsystem<UOmnisCharacterLifeSubsystem>()
        : nullptr)
    {
        Life->UnregisterNPC(this);
    }

    Super::EndPlay(EndPlayReason);
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
    DOREPLIFETIME(AOmnisNPCCharacter, LastGeneratedDialogue);
    DOREPLIFETIME(AOmnisNPCCharacter, bLastGeneratedDialogueSucceeded);
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
    WorkDaysMask = FMath::Clamp(Definition->WorkDaysMask, 0, 0x7F);
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

void AOmnisNPCCharacter::SimulateLifeFromPopulationManager(
    float DeltaSeconds,
    bool bAllowNavigation)
{
    if (!HasAuthority() || !CharacterState || !CharacterState->IsAlive() ||
        !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f)
    {
        return;
    }

    SimulateLife(DeltaSeconds, bAllowNavigation);
}

void AOmnisNPCCharacter::SimulateLife(float DeltaSeconds, bool bAllowNavigation)
{
    if (!CharacterState || !CharacterState->IsAlive())
    {
        return;
    }

    float GameHour = 12.0f;
    int32 DayOfWeek = 0;
    if (UWorld* World = GetWorld())
    {
        if (UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>())
        {
            const FOmnisWorldCalendarDate Date = Time->GetCalendarDate();
            GameHour = static_cast<float>(Date.Hour) +
                (static_cast<float>(Date.Minute) / 60.0f);
            DayOfWeek = FMath::Clamp(Date.DayOfWeek, 0, 6);
        }
    }
    const bool bIsScheduledWorkday =
        (FMath::Clamp(WorkDaysMask, 0, 0x7F) & (1 << DayOfWeek)) != 0;

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

        if (bIsScheduledWorkday &&
            IsWithinHourWindow(GameHour, WorkStartHour, WorkEndHour))
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

    if (!bAllowNavigation)
    {
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            AIController->StopMovement();
        }
        bHasLifeDestination = false;
    }
    else if (bGoalChanged || bDestinationChanged)
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
        SpreadRumorsTo(SocialTarget);

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

void AOmnisNPCCharacter::SpreadRumorsTo(AActor* Target)
{
    if (!HasAuthority() || !Target || Target == this || !CharacterState)
    {
        return;
    }

    UOmnisCharacterStateComponent* TargetState = nullptr;
    const bool bTargetIsPlayer = Cast<AOmnisPlayerCharacter>(Target) != nullptr;
    if (AOmnisNPCCharacter* TargetNPC = Cast<AOmnisNPCCharacter>(Target))
    {
        TargetState = TargetNPC->CharacterState;
    }
    else if (AOmnisPlayerCharacter* TargetPlayer = Cast<AOmnisPlayerCharacter>(Target))
    {
        TargetState = TargetPlayer->CharacterState;
    }

    if (!TargetState || TargetState == CharacterState)
    {
        return;
    }

    // Rumors are bounded by strength and by one receipt per event/person.
    // Each social exchange reduces importance, so they eventually stop spreading.
    constexpr int32 RumorSpreadLoss = 20;
    constexpr int32 MinimumRumorStrength = 30;
    const TArray<FOmnisCharacterMemoryEntry> SourceMemories = CharacterState->GetMemories();

    for (const FOmnisCharacterMemoryEntry& Memory : SourceMemories)
    {
        if (!Memory.EventId.ToString().StartsWith(TEXT("Mission.Failure.")) ||
            Memory.Importance <= MinimumRumorStrength ||
            TargetState->HasMemory(Memory.EventId))
        {
            continue;
        }

        const int32 ReceivedStrength = FMath::Max(
            MinimumRumorStrength,
            Memory.Importance - RumorSpreadLoss);
        TargetState->RecordMemory(
            Memory.EventId,
            Memory.RelatedCharacterId,
            ReceivedStrength);

        if (!bTargetIsPlayer && !Memory.RelatedCharacterId.IsNone())
        {
            TargetState->AdjustRelationship(Memory.RelatedCharacterId, -5);
        }
    }
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
    if (CharacterState && CharacterState->HasMemoryPrefix(TEXT("Mission.Failure.")))
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

    RequestGeneratedDialogue(
        Interactor,
        TEXT("The player approaches and greets you. Respond naturally using only what you know."));

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


void AOmnisNPCCharacter::RequestGeneratedDialogue(
    AActor* Interactor,
    const FString& PlayerMessage)
{
    if (!HasAuthority() || !IsValid(Interactor) || Interactor == this ||
        !CharacterState || !CharacterState->IsAlive())
    {
        return;
    }

    UGameInstance* GameInstance = GetGameInstance();
    UOmnisNPCDialogueSubsystem* Dialogue =
        GameInstance ? GameInstance->GetSubsystem<UOmnisNPCDialogueSubsystem>() : nullptr;
    if (!Dialogue)
    {
        LastGeneratedDialogue = TEXT("The character has nothing more to say right now.");
        bLastGeneratedDialogueSucceeded = false;
        ForceNetUpdate();
        OnGeneratedDialogue.Broadcast(this, false, LastGeneratedDialogue);
        return;
    }

    TWeakObjectPtr<AOmnisNPCCharacter> WeakThis(this);
    Dialogue->RequestDialogue(
        this,
        PlayerMessage,
        [WeakThis](bool bSucceeded, const FString& GeneratedText)
        {
            AOmnisNPCCharacter* NPC = WeakThis.Get();
            if (!NPC || !NPC->HasAuthority())
            {
                return;
            }

            NPC->LastGeneratedDialogue = GeneratedText;
            NPC->bLastGeneratedDialogueSucceeded = bSucceeded;
            NPC->ForceNetUpdate();
            NPC->OnGeneratedDialogue.Broadcast(NPC, bSucceeded, GeneratedText);
        });
}

void AOmnisNPCCharacter::OnRep_GeneratedDialogue()
{
    OnGeneratedDialogue.Broadcast(
        this,
        bLastGeneratedDialogueSucceeded,
        LastGeneratedDialogue);
}
