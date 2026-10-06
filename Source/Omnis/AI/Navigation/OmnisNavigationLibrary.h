#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisNavigationLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisNavigationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|AI")
    static bool FindReachableLocation(UObject* WorldContextObject, FVector Origin, FVector Direction, float Distance, FVector& OutLocation);
};
