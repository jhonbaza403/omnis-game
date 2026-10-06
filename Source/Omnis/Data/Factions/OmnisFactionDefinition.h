#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "OmnisFactionDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisFactionDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Faction")
    FName FactionId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Faction")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Faction")
    FLinearColor ThemeColor = FLinearColor::White;
};
