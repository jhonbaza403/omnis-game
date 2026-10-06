#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisInteractionComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisInteractionComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Interaction")
    bool Interact();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    float InteractionDistance = 300.0f;
};
