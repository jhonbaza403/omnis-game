#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisAutomationLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisAutomationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|Automation")
    static bool IsValidIdentifier(FName Identifier);
};
