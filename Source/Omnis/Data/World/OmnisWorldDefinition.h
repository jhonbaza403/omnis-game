#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OmnisWorldDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisWorldDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World")
    FName WorldId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="World")
    int32 CellSize = 10000;
};
