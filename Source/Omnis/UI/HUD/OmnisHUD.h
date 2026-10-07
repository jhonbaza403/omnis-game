#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OmnisHUD.generated.h"

class UOmnisHUDWidget;

UCLASS()
class OMNIS_API AOmnisHUD : public AHUD
{
    GENERATED_BODY()

public:
    AOmnisHUD();

protected:
    virtual void BeginPlay() override;
    virtual void DrawHUD() override;

private:
    static FString PhaseToText(uint8 PhaseValue);

    UPROPERTY()
    TObjectPtr<UOmnisHUDWidget> HUDWidget;
};
