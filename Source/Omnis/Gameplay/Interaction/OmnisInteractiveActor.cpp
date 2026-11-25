#include "Gameplay/Interaction/OmnisInteractiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Narrative/Decisions/OmnisDecisionSubsystem.h"
#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"
#include "Narrative/Reputation/OmnisFactionSubsystem.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

AOmnisInteractiveActor::AOmnisInteractiveActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Prompt = FText::FromString(TEXT("Interact"));
    InteractionId = NAME_None;
    SetReplicates(true);
    Tags.Add(TEXT("Omnis.Discoverable"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        Mesh->SetStaticMesh(Cube);
    }
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AOmnisInteractiveActor::BeginPlay()
{
    Super::BeginPlay();
    if (InteractionId.IsNone())
    {
        InteractionId = GetFName();
    }
}

void AOmnisInteractiveActor::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AOmnisInteractiveActor, bActivated);
    DOREPLIFETIME(AOmnisInteractiveActor, bConsumed);
}

void AOmnisInteractiveActor::Interact_Implementation(AActor* Interactor)
{
    if (!HasAuthority() || !IsValid(Interactor) || !GetWorld())
    {
        return;
    }

    AOmnisPlayerCharacter* Player = Cast<AOmnisPlayerCharacter>(Interactor);
    if (!Player || (bOneShot && bConsumed))
    {
        return;
    }

    const double Now = GetWorld()->GetTimeSeconds();
    const double Cooldown = FMath::Clamp<double>(
        static_cast<double>(FMath::IsFinite(InteractionCooldownSeconds)
            ? InteractionCooldownSeconds
            : 2.0f),
        0.1,
        30.0);
    if (Now - LastInteractionTimeSeconds < Cooldown)
    {
        return;
    }

    LastInteractionTimeSeconds = Now;
    bActivated = !bActivated;
    bConsumed = bOneShot;

    if (!bRewardClaimed)
    {
        bRewardClaimed = true;
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UOmnisProgressionSubsystem* Progression =
                GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
            {
                Progression->AddExperience(FMath::Clamp(RewardExperience, 0, 1000));
            }

            if (UOmnisDecisionSubsystem* Decisions =
                GameInstance->GetSubsystem<UOmnisDecisionSubsystem>())
            {
                Decisions->RecordDecision(InteractionId, 1);
            }

            if (UOmnisConsequenceSubsystem* Consequences =
                GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
            {
                Consequences->ApplyConsequence(InteractionId);
            }

            if (UOmnisFactionSubsystem* Factions =
                GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
            {
                if (!RewardFactionId.IsNone() && RewardFactionStanding != 0)
                {
                    Factions->AdjustStanding(
                        RewardFactionId,
                        FMath::Clamp(RewardFactionStanding, -25, 25));
                }
            }
        }
    }

    if (Player->Exploration)
    {
        Player->Exploration->Scan();
    }

    ForceNetUpdate();
    OnInteractionActivated.Broadcast(Interactor, InteractionId, bActivated);
}

void AOmnisInteractiveActor::OnRep_InteractionState()
{
    OnInteractionActivated.Broadcast(nullptr, InteractionId, bActivated);
}
