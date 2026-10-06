#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "OmnisCycleDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisCycleDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cycle")
    int32 CycleIndex = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cycle")
    float InitialWorldAge = 0.0f;
};
