#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisStatusEffectComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisStatusEffectComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisStatusEffectComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Status")
    void ApplyEffect(FName EffectId, float Duration);

    UFUNCTION(BlueprintCallable, Category="Omnis|Status")
    void ClearEffect(FName EffectId);

    UFUNCTION(BlueprintPure, Category="Omnis|Status")
    bool HasEffect(FName EffectId) const;

private:
    UPROPERTY()
    TMap<FName, float> EffectExpiry;
};
