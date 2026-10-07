#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisAtmosphereFXActor.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UPointLightComponent;

UCLASS()
class OMNIS_API AOmnisAtmosphereFXActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisAtmosphereFXActor();

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Atmosphere")
    int32 RainParticleCount = 180;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Atmosphere")
    float RainHeight = 1400.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Rain;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UPointLightComponent> StormLight;

private:
    TArray<float> RainOffsets;
    float TimeSeconds = 0.0f;
    void InitializeRain();
};
