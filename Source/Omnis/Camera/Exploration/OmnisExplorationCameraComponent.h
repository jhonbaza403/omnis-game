#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "OmnisExplorationCameraComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisExplorationCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UOmnisExplorationCameraComponent();
};
