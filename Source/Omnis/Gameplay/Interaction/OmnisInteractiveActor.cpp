#include "Gameplay/Interaction/OmnisInteractiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Narrative/Decisions/OmnisDecisionSubsystem.h"
#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"
#include "Narrative/Reputation/OmnisFactionSubsystem.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"

AOmnisInteractiveActor::AOmnisInteractiveActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Prompt = FText::FromString(TEXT("Interact"));
    InteractionId = TEXT("Omnis.Interaction");
    Tags.Add(TEXT("Omnis.Discoverable"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        Mesh->SetStaticMesh(Cube);
    }
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AOmnisInteractiveActor::Interact_Implementation(AActor* Interactor)
{
    if (AOmnisPlayerCharacter* Player = Cast<AOmnisPlayerCharacter>(Interactor))
    {
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UOmnisProgressionSubsystem* Progression = GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
            {
                Progression->AddExperience(25);
            }

            if (UOmnisDecisionSubsystem* Decisions = GameInstance->GetSubsystem<UOmnisDecisionSubsystem>())
            {
                Decisions->RecordDecision(InteractionId, 1);
            }

            if (UOmnisConsequenceSubsystem* Consequences = GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
            {
                Consequences->ApplyConsequence(InteractionId);
            }

            if (UOmnisFactionSubsystem* Factions = GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
            {
                Factions->AdjustStanding(TEXT("Wanderers"), 5);
            }
        }

        if (Player->Exploration)
        {
            Player->Exploration->Scan();
        }
    }
}
