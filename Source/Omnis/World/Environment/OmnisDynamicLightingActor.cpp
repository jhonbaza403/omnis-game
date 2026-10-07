#include "World/Environment/OmnisDynamicLightingActor.h"

#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "EngineUtils.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"

AOmnisDynamicLightingActor::AOmnisDynamicLightingActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AOmnisDynamicLightingActor::DiscoverLights()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (!Sun.IsValid())
    {
        for (TActorIterator<ADirectionalLight> It(World); It; ++It)
        {
            Sun = *It;
            break;
        }
    }

    if (!Sky.IsValid())
    {
        for (TActorIterator<ASkyLight> It(World); It; ++It)
        {
            Sky = *It;
            break;
        }
    }
}

void AOmnisDynamicLightingActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    DiscoverLights();

    TimeOfDay = FMath::Fmod(
        TimeOfDay + DeltaSeconds / FMath::Max(60.0f, DayLengthSeconds),
        1.0f
    );

    const float SunAngle = TimeOfDay * 360.0f - 90.0f;
    const float DayFactor = FMath::Clamp(FMath::Sin(FMath::DegreesToRadians(SunAngle)) * 0.5f + 0.5f, 0.05f, 1.0f);

    if (Sun.IsValid())
    {
        Sun->SetActorRotation(FRotator(SunElevation - SunAngle, -30.0f, 0.0f));

        if (UDirectionalLightComponent* Component = Sun->GetDirectionalLightComponent())
        {
            Component->SetIntensity(2.0f + 7.0f * DayFactor);
            Component->SetLightColor(FLinearColor(
                1.0f,
                FMath::Lerp(0.55f, 1.0f, DayFactor),
                FMath::Lerp(0.42f, 1.0f, DayFactor)
            ));
        }
    }

    if (Sky.IsValid())
    {
        if (USkyLightComponent* Component = Sky->GetLightComponent())
        {
            Component->SetIntensity(0.2f + 1.5f * DayFactor);
        }
    }
}
