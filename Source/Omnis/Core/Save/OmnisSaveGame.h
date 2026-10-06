#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/Cycle/OmnisCycleTypes.h"
#include "OmnisSaveGame.generated.h"

UCLASS()
class OMNIS_API UOmnisSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Save")
    FOmnisCycleState CycleState;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Save")
    int32 PermanentUpgradePoints = 0;
};
