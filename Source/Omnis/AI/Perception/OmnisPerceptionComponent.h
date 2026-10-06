#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisPerceptionComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisPerceptionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisPerceptionComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|AI")
    AActor* SenseClosest(float Radius = 1200.0f);

    UFUNCTION(BlueprintPure, Category="Omnis|AI")
    AActor* GetLastSensedActor() const { return LastSensed.Get(); }

private:
    UPROPERTY()
    TWeakObjectPtr<AActor> LastSensed;
};
