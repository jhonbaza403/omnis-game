#include "World/Environment/OmnisNexusVisualActor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
    void SetVisualMaterial(UStaticMeshComponent* Mesh, const FLinearColor& Color)
    {
        if (!Mesh)
        {
            return;
        }

        UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(
            nullptr,
            TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")
        );

        if (BaseMaterial)
        {
            if (UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(BaseMaterial, Mesh))
            {
                Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
                Material->SetVectorParameterValue(TEXT("EmissiveColor"), Color * 0.25f);
                Mesh->SetMaterial(0, Material);
            }
        }
    }
}

AOmnisNexusVisualActor::AOmnisNexusVisualActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.0f;

    CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
    RootComponent = CoreMesh;

    RingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMesh"));
    RingMesh->SetupAttachment(RootComponent);

    CoreLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("CoreLight"));
    CoreLight->SetupAttachment(RootComponent);
    CoreLight->SetCastShadows(false);
    CoreLight->SetLightColor(FLinearColor(0.46f, 0.2f, 1.0f));
    CoreLight->SetIntensity(3600.0f);
    CoreLight->SetAttenuationRadius(900.0f);
    BaseLightIntensity = 3600.0f;

    static UStaticMesh* Sphere = LoadObject<UStaticMesh>(
        nullptr,
        TEXT("/Engine/BasicShapes/Sphere.Sphere")
    );
    static UStaticMesh* Cylinder = LoadObject<UStaticMesh>(
        nullptr,
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder")
    );

    if (Sphere)
    {
        CoreMesh->SetStaticMesh(Sphere);
        CoreMesh->SetRelativeScale3D(FVector(2.15f));
        SetVisualMaterial(CoreMesh, FLinearColor(0.38f, 0.14f, 0.9f));
    }

    if (Cylinder)
    {
        RingMesh->SetStaticMesh(Cylinder);
        RingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        RingMesh->SetRelativeScale3D(FVector(3.4f, 3.4f, 0.12f));
        RingMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -6.0f));
        SetVisualMaterial(RingMesh, FLinearColor(0.05f, 0.7f, 1.0f));
    }
}

void AOmnisNexusVisualActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ElapsedTime += DeltaSeconds;

    if (RingMesh)
    {
        FRotator RingRotation = RingMesh->GetRelativeRotation();
        RingRotation.Yaw += RotationSpeed * DeltaSeconds;
        RingRotation.Roll += (RotationSpeed * 0.35f) * DeltaSeconds;
        RingMesh->SetRelativeRotation(RingRotation);
    }

    const float Pulse = 1.0f + FMath::Sin(ElapsedTime * PulseSpeed) * PulseAmplitude;

    if (CoreMesh)
    {
        CoreMesh->SetRelativeScale3D(FVector(2.15f * Pulse));
    }

    if (CoreLight)
    {
        CoreLight->SetIntensity(BaseLightIntensity * Pulse);
    }
}
