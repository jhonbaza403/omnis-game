#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisProceduralAnimationComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisProceduralAnimationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisProceduralAnimationComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Animation")
    float BobAmplitude = 3.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Animation")
    float BobFrequency = 1.4f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Animation")
    float TurnAmplitude = 4.0f;

private:
    FVector BaseRelativeLocation = FVector::ZeroVector;
    float TimeSeconds = 0.0f;
};
