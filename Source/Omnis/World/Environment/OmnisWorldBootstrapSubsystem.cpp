#include "World/Environment/OmnisWorldBootstrapSubsystem.h"

#include "Engine/PointLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Gameplay/Interaction/OmnisInteractiveActor.h"
#include "AI/NPC/OmnisNPCCharacter.h"
#include "World/Environment/OmnisNexusVisualActor.h"

namespace
{
    UStaticMesh* LoadEngineMesh(const TCHAR* ObjectPath)
    {
        return LoadObject<UStaticMesh>(nullptr, ObjectPath);
    }

    UMaterialInterface* LoadBasicMaterial()
    {
        return LoadObject<UMaterialInterface>(
            nullptr,
            TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
        );
    }

    void ApplyVisualMaterial(UStaticMeshComponent* Mesh, const FLinearColor& Color, float Roughness)
    {
        if (!Mesh)
        {
            return;
        }

        if (UMaterialInterface* BaseMaterial = LoadBasicMaterial())
        {
            if (UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(BaseMaterial, Mesh))
            {
                Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
                Material->SetVectorParameterValue(TEXT("EmissiveColor"), Color * 0.15f);
                Material->SetScalarParameterValue(TEXT("Roughness"), Roughness);
                Mesh->SetMaterial(0, Material);
            }
        }
    }

    AActor* SpawnMeshActor(
        UWorld* World,
        UStaticMesh* MeshAsset,
        const FVector& Location,
        const FVector& Scale,
        const FLinearColor& Color,
        bool bCollision
    )
    {
        if (!World || !MeshAsset)
        {
            return nullptr;
        }

        AActor* Actor = World->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator);
        if (!Actor)
        {
            return nullptr;
        }

        UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Actor, TEXT("VisualMesh"));
        Actor->SetRootComponent(Mesh);
        Mesh->RegisterComponent();
        Mesh->SetStaticMesh(MeshAsset);
        Mesh->SetCollisionEnabled(
            bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision
        );
        ApplyVisualMaterial(Mesh, Color, 0.32f);
        Actor->SetActorScale3D(Scale);
        return Actor;
    }

    APointLight* SpawnPointLight(
        UWorld* World,
        const FVector& Location,
        const FLinearColor& Color,
        float Intensity,
        float Radius
    )
    {
        if (!World)
        {
            return nullptr;
        }

        APointLight* Light = World->SpawnActor<APointLight>(
            APointLight::StaticClass(),
            Location,
            FRotator::ZeroRotator
        );
        if (!Light)
        {
            return nullptr;
        }

        if (UPointLightComponent* LightComponent = Light->GetPointLightComponent())
        {
            LightComponent->SetLightColor(Color);
            LightComponent->SetIntensity(Intensity);
            LightComponent->SetAttenuationRadius(Radius);
            LightComponent->SetCastShadows(false);
        }

        return Light;
    }
}

void UOmnisWorldBootstrapSubsystem::BuildInitialWorld()
{
    if (bWorldBuilt)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World || !World->GetAuthGameMode())
    {
        return;
    }

    UStaticMesh* Cube = LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* Cylinder = LoadEngineMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (!Cube || !Cylinder)
    {
        return;
    }

    if (World->GetWorldSettings())
    {
        World->GetWorldSettings()->WorldToMeters = 100.0f;
    }

    bool bHasPlayerStart = false;
    for (TActorIterator<APlayerStart> It(World); It; ++It)
    {
        bHasPlayerStart = true;
        break;
    }

    if (!bHasPlayerStart)
    {
        APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(
            APlayerStart::StaticClass(),
            FVector(-900.0f, 0.0f, 125.0f),
            FRotator(0.0f, 0.0f, 0.0f)
        );

        if (PlayerStart)
        {
            PlayerStart->SetActorLabel(TEXT("Omnis_Runtime_PlayerStart"));
        }
    }

    SpawnMeshActor(
        World,
        Cube,
        FVector(0.0f, 0.0f, -55.0f),
        FVector(35.0f, 35.0f, 0.5f),
        FLinearColor(0.025f, 0.05f, 0.08f),
        true
    );

    const FLinearColor Cyan(0.08f, 0.8f, 1.0f);
    const FLinearColor Violet(0.42f, 0.16f, 0.95f);
    const FLinearColor Gold(1.0f, 0.55f, 0.12f);
    const FLinearColor Green(0.1f, 1.0f, 0.48f);

    bool bHasNexus = false;
    for (TActorIterator<AOmnisNexusVisualActor> It(World); It; ++It)
    {
        bHasNexus = true;
        break;
    }

    if (!bHasNexus)
    {
        if (AOmnisNexusVisualActor* Nexus = World->SpawnActor<AOmnisNexusVisualActor>(
            AOmnisNexusVisualActor::StaticClass(),
            FVector(0.0f, 0.0f, 165.0f),
            FRotator::ZeroRotator
        ))
        {
            Nexus->SetActorLabel(TEXT("Omnis_Runtime_Nexus"));
        }
    }

    const TArray<FVector> Pillars = {
        FVector(800.0f, 0.0f, 150.0f),
        FVector(-800.0f, 0.0f, 150.0f),
        FVector(0.0f, 800.0f, 150.0f),
        FVector(0.0f, -800.0f, 150.0f)
    };

    for (int32 Index = 0; Index < Pillars.Num(); ++Index)
    {
        SpawnMeshActor(
            World,
            Cylinder,
            Pillars[Index],
            FVector(1.8f, 1.8f, 2.5f),
            Index % 2 == 0 ? Cyan : Violet,
            true
        );

        SpawnPointLight(
            World,
            Pillars[Index] + FVector(0.0f, 0.0f, 210.0f),
            Index % 2 == 0 ? Cyan : Violet,
            2400.0f,
            520.0f
        );
    }

    for (int32 Ring = 0; Ring < 2; ++Ring)
    {
        const float Radius = 480.0f + Ring * 180.0f;
        for (int32 Index = 0; Index < 12; ++Index)
        {
            const float Angle = FMath::DegreesToRadians(static_cast<float>(Index) * 30.0f + Ring * 15.0f);
            const FVector Location(
                FMath::Cos(Angle) * Radius,
                FMath::Sin(Angle) * Radius,
                45.0f + Ring * 20.0f
            );

            SpawnMeshActor(
                World,
                Cylinder,
                Location,
                FVector(0.38f, 0.38f, 0.7f + Ring * 0.25f),
                Ring == 0 ? Gold : Cyan,
                true
            );
        }
    }

    const TArray<FVector> Landmarks = {
        FVector(1150.0f, 650.0f, 85.0f),
        FVector(-1150.0f, 650.0f, 85.0f),
        FVector(1150.0f, -650.0f, 85.0f),
        FVector(-1150.0f, -650.0f, 85.0f)
    };

    for (int32 Index = 0; Index < Landmarks.Num(); ++Index)
    {
        AOmnisInteractiveActor* Beacon = World->SpawnActor<AOmnisInteractiveActor>(
            Landmarks[Index],
            FRotator::ZeroRotator
        );

        if (!Beacon)
        {
            continue;
        }

        Beacon->InteractionId = FName(*FString::Printf(TEXT("Beacon.%d"), Index + 1));
        Beacon->Prompt = FText::FromString(
            FString::Printf(TEXT("Examinar hito %d"), Index + 1)
        );
        Beacon->SetActorScale3D(FVector(1.4f));

        SpawnPointLight(
            World,
            Landmarks[Index] + FVector(0.0f, 0.0f, 160.0f),
            Index % 2 == 0 ? Gold : Green,
            1800.0f,
            420.0f
        );
    }

    const TArray<FVector> NPCs = {
        FVector(280.0f, 520.0f, 115.0f),
        FVector(-360.0f, -360.0f, 115.0f)
    };

    for (const FVector& Location : NPCs)
    {
        World->SpawnActor<AOmnisNPCCharacter>(Location, FRotator::ZeroRotator);
    }

    bWorldBuilt = true;
}
