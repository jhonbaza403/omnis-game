#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisVFXActor.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisVFXActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisVFXActor();

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|VFX")
    float RotationSpeed = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|VFX")
    float PulseSpeed = 2.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|VFX")
    TObjectPtr<UStaticMeshComponent> OuterRing;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|VFX")
    TObjectPtr<UStaticMeshComponent> InnerRing;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|VFX")
    TObjectPtr<UPointLightComponent> Light;

private:
    float TimeSeconds = 0.0f;
    float BaseIntensity = 0.0f;
};
