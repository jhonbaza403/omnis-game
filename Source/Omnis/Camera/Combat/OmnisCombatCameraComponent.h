#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "OmnisCombatCameraComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisCombatCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UOmnisCombatCameraComponent();
};
