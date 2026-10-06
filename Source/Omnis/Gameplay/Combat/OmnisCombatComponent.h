#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOmnisAttackResolved, AActor*, Target, float, Damage);

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisCombatComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Combat")
    bool Attack();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Combat")
    float AttackRange = 280.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Combat")
    float AttackDamage = 25.0f;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Combat")
    FOnOmnisAttackResolved OnAttackResolved;
};
