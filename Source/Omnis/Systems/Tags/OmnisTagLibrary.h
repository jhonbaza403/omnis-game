#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmnisTagLibrary.generated.h"

UCLASS()
class OMNIS_API UOmnisTagLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|Tags")
    static bool HasAnyTag(const FGameplayTagContainer& Container, const FGameplayTagContainer& Query);

    UFUNCTION(BlueprintPure, Category="Omnis|Tags")
    static bool HasAllTags(const FGameplayTagContainer& Container, const FGameplayTagContainer& Query);
};
