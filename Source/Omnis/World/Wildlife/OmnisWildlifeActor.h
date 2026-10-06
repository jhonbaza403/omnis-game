#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisWildlifeActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisWildlifeActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisWildlifeActor();

    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Visual;

private:
    float WanderTime = 0.0f;
};
