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
#include "AI/NPC/OmnisCharacterLifeSubsystem.h"
#include "World/Environment/OmnisNexusVisualActor.h"
#include "World/Environment/OmnisVFXActor.h"
#include "World/Environment/OmnisDynamicLightingActor.h"
#include "World/Environment/OmnisAtmosphereFXActor.h"

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

    if (!FindActorOfType<AOmnisNexusVisualActor>(World))
    {
        World->SpawnActor<AOmnisNexusVisualActor>(
            AOmnisNexusVisualActor::StaticClass(),
            FVector(0,0,165),
            FRotator::ZeroRotator
        );
    }

    if (!FindActorOfType<AOmnisVFXActor>(World))
    {
        World->SpawnActor<AOmnisVFXActor>(
            AOmnisVFXActor::StaticClass(),
            FVector(0,0,165),
            FRotator::ZeroRotator
        );
    }

    if (!FindActorOfType<AOmnisDynamicLightingActor>(World))
    {
        World->SpawnActor<AOmnisDynamicLightingActor>(
            AOmnisDynamicLightingActor::StaticClass(),
            FVector::ZeroVector,
            FRotator::ZeroRotator
        );
    }

    if (!FindActorOfType<AOmnisAtmosphereFXActor>(World))
    {
        World->SpawnActor<AOmnisAtmosphereFXActor>(
            AOmnisAtmosphereFXActor::StaticClass(),
            FVector::ZeroVector,
            FRotator::ZeroRotator
        );
    }

    if (World->GetName().Contains(TEXT("Omnis_OpenWorld")))
    {
        bWorldBuilt = true;
        return;
    }

    SpawnMeshActor(World,Cube,FVector(0,0,-55),FVector(35,35,0.5f),FLinearColor(0.025f,0.05f,0.08f),true);

    const FLinearColor Cyan(0.08f,0.8f,1.0f);
    const FLinearColor Violet(0.42f,0.16f,0.95f);
    const FLinearColor Gold(1.0f,0.55f,0.12f);
    const FLinearColor Green(0.1f,1.0f,0.48f);

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
    for (int32 Index = 0; Index < NPCs.Num(); ++Index)
    {
        const FTransform SpawnTransform(FRotator::ZeroRotator, NPCs[Index]);
        AOmnisNPCCharacter* NPC = World->SpawnActorDeferred<AOmnisNPCCharacter>(
            AOmnisNPCCharacter::StaticClass(),
            SpawnTransform);
        if (!NPC)
        {
            continue;
        }

        NPC->CharacterId = TEXT("NPC.Wanderer");
        NPC->PersistentId = FName(*FString::Printf(TEXT("NPC.Bootstrap.%02d"), Index + 1));
        if (Index == 0)
        {
            NPC->CharacterDisplayName = FText::FromString(TEXT("Mara León"));
            NPC->CharacterBiography = FText::FromString(
                TEXT("Guía local que conoce los caminos del primer asentamiento y ayuda a los recién llegados."));
            NPC->OccupationId = TEXT("Occupation.LocalGuide");
            NPC->HomeLocationId = TEXT("Home.Bootstrap.Mara");
            NPC->WorkLocationId = TEXT("Work.Bootstrap.WelcomePoint");
            NPC->WorkLocationOffset = FVector(650.0f, 0.0f, 0.0f);
            NPC->WorkStartHour = 7.0f;
            NPC->WorkEndHour = 17.0f;
            NPC->RestStartHour = 21.0f;
            NPC->RestEndHour = 6.0f;
        }
        else
        {
            NPC->CharacterDisplayName = FText::FromString(TEXT("Tomás Herrera"));
            NPC->CharacterBiography = FText::FromString(
                TEXT("Técnico de campo que mantiene los equipos y las instalaciones del asentamiento."));
            NPC->OccupationId = TEXT("Occupation.FieldTechnician");
            NPC->HomeLocationId = TEXT("Home.Bootstrap.Tomas");
            NPC->WorkLocationId = TEXT("Work.Bootstrap.Maintenance");
            NPC->WorkLocationOffset = FVector(-650.0f, 0.0f, 0.0f);
            NPC->WorkStartHour = 8.0f;
            NPC->WorkEndHour = 18.0f;
            NPC->RestStartHour = 22.0f;
            NPC->RestEndHour = 6.0f;
        }
        if (UOmnisCharacterLifeSubsystem* Life =
            World->GetSubsystem<UOmnisCharacterLifeSubsystem>())
        {
            const FVector WorkPosition = NPCs[Index] + NPC->WorkLocationOffset;
            if (!Life->RegisterLocation(NPC->HomeLocationId, NPCs[Index]) ||
                !Life->RegisterLocation(NPC->WorkLocationId, WorkPosition))
            {
                UE_LOG(LogTemp, Error,
                    TEXT("OMNIS bootstrap NPC '%s' could not register home/work locations."),
                    *NPC->PersistentId.ToString());
                NPC->Destroy();
                continue;
            }
        }

        NPC->FinishSpawning(SpawnTransform);
    }

    bWorldBuilt = true;
}
