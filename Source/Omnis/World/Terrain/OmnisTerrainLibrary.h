#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisTerrainLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisTerrainLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Terrain")
    static bool FindGround(UObject* WorldContextObject, FVector Origin, float TraceHeight, FVector& OutLocation);
};
