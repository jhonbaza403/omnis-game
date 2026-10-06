#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisPhysicsComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisPhysicsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisPhysicsComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Physics")
    void SetSimulationEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category="Omnis|Physics")
    void AddImpulse(FVector Impulse);
};
