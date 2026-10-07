#include "World/Environment/OmnisAtmosphereFXActor.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Math/UnrealMathUtility.h"

AOmnisAtmosphereFXActor::AOmnisAtmosphereFXActor()
{
    PrimaryActorTick.bCanEverTick = true;

    Rain = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Rain"));
    RootComponent = Rain;
    Rain->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Rain->SetCastShadow(false);

    StormLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StormLight"));
    StormLight->SetupAttachment(RootComponent);
    StormLight->SetCastShadows(false);
    StormLight->SetLightColor(FLinearColor(0.45f, 0.65f, 1.0f));
    StormLight->SetIntensity(0.0f);
    StormLight->SetAttenuationRadius(1800.0f);
}

void AOmnisAtmosphereFXActor::InitializeRain()
{
    if (!Rain || Rain->GetInstanceCount() > 0)
    {
        return;
    }

    UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (!Cylinder)
    {
        return;
    }

    Rain->SetStaticMesh(Cylinder);
    RainOffsets.Reserve(RainParticleCount);

    FRandomStream Stream(32451);
    for (int32 Index = 0; Index < RainParticleCount; ++Index)
    {
        const FVector Location(
            Stream.FRandRange(-4200.0f, 4200.0f),
            Stream.FRandRange(-4200.0f, 4200.0f),
            Stream.FRandRange(100.0f, RainHeight)
        );

        Rain->AddInstance(FTransform(
            FRotator(0.0f, 0.0f, 0.0f),
            Location,
            FVector(0.02f, 0.02f, Stream.FRandRange(0.25f, 0.65f))
        ));

        RainOffsets.Add(Location.Z);
    }
}

void AOmnisAtmosphereFXActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    InitializeRain();
    if (!Rain) return;

    TimeSeconds += DeltaSeconds;

    for (int32 Index = 0; Index < Rain->GetInstanceCount() && Index < RainOffsets.Num(); ++Index)
    {
        FTransform Transform;
        if (!Rain->GetInstanceTransform(Index, Transform, true))
        {
            continue;
        }

        FVector Location = Transform.GetLocation();
        Location.Z -= 1050.0f * DeltaSeconds;

        if (Location.Z < 0.0f)
        {
            Location.Z = RainHeight + FMath::Fmod(static_cast<float>(Index) * 37.0f, 600.0f);
        }

        Transform.SetLocation(Location);
        Rain->UpdateInstanceTransform(Index, Transform, true, false, true);
    }

    if (StormLight)
    {
        const float Flicker = FMath::Pow(FMath::Max(0.0f, FMath::Sin(TimeSeconds * 0.37f)), 32.0f);
        StormLight->SetIntensity(Flicker * 8500.0f);
    }

    Rain->MarkRenderStateDirty();
}
