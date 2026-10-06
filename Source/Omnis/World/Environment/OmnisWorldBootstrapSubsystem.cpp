#include "World/Environment/OmnisWorldBootstrapSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DirectionalLight.h"
#include "Gameplay/Interaction/OmnisInteractiveActor.h"
#include "AI/NPC/OmnisNPCCharacter.h"
#include "Systems/Damage/OmnisHealthComponent.h"

void UOmnisWorldBootstrapSubsystem::BuildInitialWorld()
{
    if (bWorldBuilt)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    static UStaticMesh* Cube = nullptr;
    if (!Cube)
    {
        Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    }

    AActor* Floor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector(0.0f, 0.0f, -60.0f), FRotator::ZeroRotator);
    if (Floor)
    {
        UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Floor, TEXT("FloorMesh"));
        Mesh->RegisterComponent();
        Floor->SetRootComponent(Mesh);
        Mesh->SetStaticMesh(Cube);
        Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Floor->SetActorScale3D(FVector(30.0f, 30.0f, 0.5f));
    }

    if (ADirectionalLight* Light = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0.0f), FRotator(-45.0f, -30.0f, 0.0f)))
    {
        Light->GetLightComponent()->SetIntensity(5.0f);
    }

    const TArray<FVector> InteractiveLocations = {
        FVector(600.0f, 100.0f, 60.0f),
        FVector(600.0f, -350.0f, 60.0f),
        FVector(-500.0f, 450.0f, 60.0f)
    };

    for (int32 Index = 0; Index < InteractiveLocations.Num(); ++Index)
    {
        AOmnisInteractiveActor* Beacon = World->SpawnActor<AOmnisInteractiveActor>(InteractiveLocations[Index], FRotator::ZeroRotator);
        if (Beacon)
        {
            Beacon->InteractionId = FName(*FString::Printf(TEXT("Beacon.%d"), Index + 1));
            Beacon->Prompt = FText::FromString(FString::Printf(TEXT("Explore landmark %d"), Index + 1));
            Beacon->SetActorScale3D(FVector(0.8f));
        }
    }

    const TArray<FVector> NPCs = {
        FVector(250.0f, 500.0f, 110.0f),
        FVector(-350.0f, -300.0f, 110.0f)
    };

    for (const FVector& Location : NPCs)
    {
        World->SpawnActor<AOmnisNPCCharacter>(Location, FRotator::ZeroRotator);
    }

    bWorldBuilt = true;
}
