#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "OmnisCinematicCameraComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisCinematicCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UOmnisCinematicCameraComponent();
};
