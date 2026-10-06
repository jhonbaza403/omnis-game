#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OmnisItemDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
    FName ItemId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
    int32 MaxStack = 99;
};
