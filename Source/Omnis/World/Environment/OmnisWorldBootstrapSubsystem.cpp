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
#include "World/Environment/OmnisVFXActor.h"

namespace
{
    UStaticMesh* LoadEngineMesh(const TCHAR* ObjectPath)
    {
        return LoadObject<UStaticMesh>(nullptr, ObjectPath);
    }

    UMaterialInterface* LoadBasicMaterial()
    {
        return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    }

    void ApplyVisualMaterial(UStaticMeshComponent* Mesh, const FLinearColor& Color, float Roughness)
    {
        if (!Mesh) return;
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

    AActor* SpawnMeshActor(UWorld* World,UStaticMesh* MeshAsset,const FVector& Location,const FVector& Scale,const FLinearColor& Color,bool bCollision)
    {
        if (!World || !MeshAsset) return nullptr;
        AActor* Actor = World->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator);
        if (!Actor) return nullptr;
        UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Actor, TEXT("VisualMesh"));
        Actor->SetRootComponent(Mesh);
        Mesh->RegisterComponent();
        Mesh->SetStaticMesh(MeshAsset);
        Mesh->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
        ApplyVisualMaterial(Mesh, Color, 0.32f);
        Actor->SetActorScale3D(Scale);
        return Actor;
    }

    APointLight* SpawnPointLight(UWorld* World,const FVector& Location,const FLinearColor& Color,float Intensity,float Radius)
    {
        if (!World) return nullptr;
        APointLight* Light = World->SpawnActor<APointLight>(APointLight::StaticClass(), Location, FRotator::ZeroRotator);
        if (!Light) return nullptr;
        if (UPointLightComponent* Component = Light->GetPointLightComponent())
        {
            Component->SetLightColor(Color);
            Component->SetIntensity(Intensity);
            Component->SetAttenuationRadius(Radius);
            Component->SetCastShadows(false);
        }
        return Light;
    }

    template<typename TActor>
    TActor* FindActorOfType(UWorld* World)
    {
        if (!World) return nullptr;
        for (TActorIterator<TActor> It(World); It; ++It)
        {
            return *It;
        }
        return nullptr;
    }
}

void UOmnisWorldBootstrapSubsystem::BuildInitialWorld()
{
    if (bWorldBuilt) return;

    UWorld* World = GetWorld();
    if (!World || !World->GetAuthGameMode()) return;

    UStaticMesh* Cube = LoadEngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* Cylinder = LoadEngineMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (!Cube || !Cylinder) return;

    if (World->GetWorldSettings()) World->GetWorldSettings()->WorldToMeters = 100.0f;

    if (!FindActorOfType<APlayerStart>(World))
    {
        World->SpawnActor<APlayerStart>(
            APlayerStart::StaticClass(),
            FVector(-900.0f,0.0f,125.0f),
            FRotator::ZeroRotator
        );
    }

    const FLinearColor Cyan(0.08f,0.8f,1.0f);
    const FLinearColor Violet(0.42f,0.16f,0.95f);
    const FLinearColor Gold(1.0f,0.55f,0.12f);
    const FLinearColor Green(0.1f,1.0f,0.48f);

    // Nexus and VFX remain dynamic even on the persistent authored map.
    if (!FindActorOfType<AOmnisNexusVisualActor>(World))
    {
        if (AOmnisNexusVisualActor* Nexus = World->SpawnActor<AOmnisNexusVisualActor>(
            AOmnisNexusVisualActor::StaticClass(),
            FVector(0,0,165),
            FRotator::ZeroRotator))
        {
            Nexus->SetActorLabel(TEXT("Omnis_Runtime_Nexus"));
        }
    }

    if (!FindActorOfType<AOmnisVFXActor>(World))
    {
        if (AOmnisVFXActor* VFX = World->SpawnActor<AOmnisVFXActor>(
            AOmnisVFXActor::StaticClass(),
            FVector(0,0,165),
            FRotator::ZeroRotator))
        {
            VFX->SetActorLabel(TEXT("Omnis_Runtime_Nexus_VFX"));
        }
    }

    const bool bPersistentWorld = World->GetName().Contains(TEXT("Omnis_OpenWorld"));
    if (bPersistentWorld)
    {
        bWorldBuilt = true;
        return;
    }

    SpawnMeshActor(World,Cube,FVector(0,0,-55),FVector(35,35,0.5f),FLinearColor(0.025f,0.05f,0.08f),true);

    const TArray<FVector> Pillars = {
        FVector(800,0,150),FVector(-800,0,150),FVector(0,800,150),FVector(0,-800,150)
    };

    for (int32 Index=0;Index<Pillars.Num();++Index)
    {
        SpawnMeshActor(World,Cylinder,Pillars[Index],FVector(1.8f,1.8f,2.5f),Index%2==0?Cyan:Violet,true);
        SpawnPointLight(World,Pillars[Index]+FVector(0,0,210),Index%2==0?Cyan:Violet,2400,520);
    }

    const TArray<FVector> Landmarks = {
        FVector(1150,650,85),FVector(-1150,650,85),FVector(1150,-650,85),FVector(-1150,-650,85)
    };

    for (int32 Index=0;Index<Landmarks.Num();++Index)
    {
        AOmnisInteractiveActor* Beacon = World->SpawnActor<AOmnisInteractiveActor>(Landmarks[Index],FRotator::ZeroRotator);
        if (!Beacon) continue;
        Beacon->InteractionId=FName(*FString::Printf(TEXT("Beacon.%d"),Index+1));
        Beacon->Prompt=FText::FromString(FString::Printf(TEXT("Examinar hito %d"),Index+1));
        Beacon->SetActorScale3D(FVector(1.4f));
        SpawnPointLight(World,Landmarks[Index]+FVector(0,0,160),Index%2==0?Gold:Green,1800,420);
    }

    const TArray<FVector> NPCs = {FVector(280,520,115),FVector(-360,-360,115)};
    for (const FVector& Location : NPCs)
    {
        World->SpawnActor<AOmnisNPCCharacter>(Location,FRotator::ZeroRotator);
    }

    bWorldBuilt = true;
}
