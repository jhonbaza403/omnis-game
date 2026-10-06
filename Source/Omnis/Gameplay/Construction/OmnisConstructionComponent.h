#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisConstructionComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisConstructionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisConstructionComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Construction")
    AActor* ConstructForward();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Construction")
    float BuildDistance = 220.0f;
};
