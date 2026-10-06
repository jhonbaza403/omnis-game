#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OmnisGameMode.generated.h"

class AController;

UCLASS()
class OMNIS_API AOmnisGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AOmnisGameMode();

    virtual void BeginPlay() override;
    virtual void RestartPlayer(AController* NewPlayer) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Cycle")
    void NotifyPlayerDeath(AController* PlayerController);

private:
    void RespawnAfterNexus(AController* PlayerController);

    FTimerHandle RespawnTimer;
};
