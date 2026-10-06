#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisAbilityComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisAbilityComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Abilities")
    bool ActivateAbility(FName AbilityId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Abilities")
    void SetCooldown(FName AbilityId, float Duration);

    UFUNCTION(BlueprintPure, Category="Omnis|Abilities")
    bool IsOnCooldown(FName AbilityId) const;

private:
    UPROPERTY()
    TMap<FName, float> Cooldowns;
};
