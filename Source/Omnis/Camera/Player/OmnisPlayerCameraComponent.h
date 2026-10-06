#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "OmnisPlayerCameraComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisPlayerCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UOmnisPlayerCameraComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Camera")
    void SetDistance(float Distance);
};
