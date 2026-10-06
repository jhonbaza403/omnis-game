#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OmnisPlayerController.generated.h"

UCLASS()
class OMNIS_API AOmnisPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AOmnisPlayerController();

protected:
    virtual void BeginPlay() override;
};
