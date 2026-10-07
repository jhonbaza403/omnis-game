#include "World/Environment/OmnisVFXActor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
    void ApplyFXMaterial(UStaticMeshComponent* Mesh, const FLinearColor& Color)
    {
        if (!Mesh)
        {
            return;
        }

        UMaterialInterface* Material = LoadObject<UMaterialInterface>(
            nullptr,
            TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
        );

        if (Material)
        {
            if (UMaterialInstanceDynamic* Instance = UMaterialInstanceDynamic::Create(Material, Mesh))
            {
                Instance->SetVectorParameterValue(TEXT("BaseColor"), Color);
                Instance->SetVectorParameterValue(TEXT("EmissiveColor"), Color * 0.4f);
                Mesh->SetMaterial(0, Instance);
            }
        }
    }
}

AOmnisVFXActor::AOmnisVFXActor()
{
    PrimaryActorTick.bCanEverTick = true;

    OuterRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OuterRing"));
    RootComponent = OuterRing;
    InnerRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InnerRing"));
    InnerRing->SetupAttachment(RootComponent);
    Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
    Light->SetupAttachment(RootComponent);
    Light->SetCastShadows(false);
    Light->SetLightColor(FLinearColor(0.12f, 0.8f, 1.0f));
    Light->SetIntensity(1800.0f);
    Light->SetAttenuationRadius(480.0f);
    BaseIntensity = 1800.0f;

    UStaticMesh* Cylinder = LoadObject<UStaticMesh>(
        nullptr,
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder")
    );

    if (Cylinder)
    {
        OuterRing->SetStaticMesh(Cylinder);
        OuterRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        OuterRing->SetRelativeScale3D(FVector(1.5f, 1.5f, 0.08f));
        ApplyFXMaterial(OuterRing, FLinearColor(0.05f, 0.55f, 1.0f));

        InnerRing->SetStaticMesh(Cylinder);
        InnerRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        InnerRing->SetRelativeScale3D(FVector(0.72f, 0.72f, 0.14f));
        InnerRing->SetRelativeLocation(FVector(0.0f, 0.0f, 18.0f));
        ApplyFXMaterial(InnerRing, FLinearColor(0.4f, 0.12f, 0.95f));
    }
}

void AOmnisVFXActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    TimeSeconds += DeltaSeconds;

    if (OuterRing)
    {
        FRotator Rotation = OuterRing->GetRelativeRotation();
        Rotation.Yaw += RotationSpeed * DeltaSeconds;
        OuterRing->SetRelativeRotation(Rotation);
    }

    if (InnerRing)
    {
        FRotator Rotation = InnerRing->GetRelativeRotation();
        Rotation.Yaw -= RotationSpeed * 1.6f * DeltaSeconds;
        InnerRing->SetRelativeRotation(Rotation);

        const float Pulse = 1.0f + 0.2f * FMath::Sin(TimeSeconds * PulseSpeed);
        InnerRing->SetRelativeScale3D(FVector(0.72f * Pulse, 0.72f * Pulse, 0.14f));
    }

    if (Light)
    {
        Light->SetIntensity(BaseIntensity * (1.0f + 0.35f * FMath::Sin(TimeSeconds * PulseSpeed)));
    }
}
