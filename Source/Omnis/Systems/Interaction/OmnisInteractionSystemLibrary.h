#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisInteractionSystemLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisInteractionSystemLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|Interaction")
    static bool IsInteractable(AActor* Actor);
};
