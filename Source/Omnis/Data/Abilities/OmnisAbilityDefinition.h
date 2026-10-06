#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OmnisAbilityDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisAbilityDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
    FName AbilityId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
    float Cooldown = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
    float Power = 1.0f;
};
