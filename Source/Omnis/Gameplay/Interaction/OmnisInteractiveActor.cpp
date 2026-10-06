#include "Gameplay/Interaction/OmnisInteractiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"

AOmnisInteractiveActor::AOmnisInteractiveActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Prompt = FText::FromString(TEXT("Interact"));
    InteractionId = TEXT("Omnis.Interaction");
    Tags.Add(TEXT("Omnis.Discoverable"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        Mesh->SetStaticMesh(MeshAsset.Object);
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
        }
        Player->Exploration->Scan();
    }
}
