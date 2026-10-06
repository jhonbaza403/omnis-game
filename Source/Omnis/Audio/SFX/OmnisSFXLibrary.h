#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisSFXLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisSFXLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|Audio")
    static float ComputeAudibility(float BaseVolume, float Distance, float MaxDistance);
};
