#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisNexusVisualActor.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisNexusVisualActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisNexusVisualActor();

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Visual")
    float RotationSpeed = 22.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Visual")
    float PulseSpeed = 1.4f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Visual")
    float PulseAmplitude = 0.28f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Visual")
    TObjectPtr<UStaticMeshComponent> CoreMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Visual")
    TObjectPtr<UStaticMeshComponent> RingMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Visual")
    TObjectPtr<UPointLightComponent> CoreLight;

private:
    float ElapsedTime = 0.0f;
    float BaseLightIntensity = 0.0f;
};
