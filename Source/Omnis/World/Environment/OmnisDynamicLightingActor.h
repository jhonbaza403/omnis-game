#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisDynamicLightingActor.generated.h"

class ADirectionalLight;
class ASkyLight;

UCLASS()
class OMNIS_API AOmnisDynamicLightingActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisDynamicLightingActor();

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Lighting")
    float DayLengthSeconds = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Lighting")
    float SunElevation = -35.0f;

private:
    TWeakObjectPtr<ADirectionalLight> Sun;
    TWeakObjectPtr<ASkyLight> Sky;
    float TimeOfDay = 0.25f;

    void DiscoverLights();
};
