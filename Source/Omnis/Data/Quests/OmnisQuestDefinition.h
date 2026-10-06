#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OmnisQuestDefinition.generated.h"

UCLASS(BlueprintType)
class OMNIS_API UOmnisQuestDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
    FName QuestId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
    FText Title;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
    int32 ExperienceReward = 100;
};
