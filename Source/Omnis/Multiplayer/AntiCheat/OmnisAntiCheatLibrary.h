#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisAntiCheatLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisAntiCheatLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|AntiCheat")
    static bool IsFiniteFloat(float Value);

    UFUNCTION(BlueprintPure, Category="Omnis|AntiCheat")
    static float ClampClientFloat(float Value, float MinValue, float MaxValue);
};
