#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OmnisGameMode.generated.h"

UCLASS()
class OMNIS_API AOmnisGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AOmnisGameMode();
    virtual void BeginPlay() override;
};
