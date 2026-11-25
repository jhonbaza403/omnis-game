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

    virtual void SetupInputComponent() override;

protected:
    virtual void BeginPlay() override;

private:
    void ToggleNearestVehicle();
    void ToggleNearestAircraft();

    UFUNCTION(Server, Reliable)
    void ServerToggleNearestVehicle();

    UFUNCTION(Server, Reliable)
    void ServerToggleNearestAircraft();
    APawn* FindNearestPawn(UClass* PawnClass, float MaxDistance) const;

    TWeakObjectPtr<APawn> OnFootPawn;
};
