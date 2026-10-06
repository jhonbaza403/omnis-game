#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisAttributeComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisAttributeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisAttributeComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Attributes")
    void SetAttribute(FName AttributeId, float Value);

    UFUNCTION(BlueprintCallable, Category="Omnis|Attributes")
    void AddAttribute(FName AttributeId, float Delta);

    UFUNCTION(BlueprintPure, Category="Omnis|Attributes")
    float GetAttribute(FName AttributeId, float DefaultValue = 0.0f) const;

private:
    UPROPERTY()
    TMap<FName, float> Attributes;
};
