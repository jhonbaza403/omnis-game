#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OmnisInteractableInterface.generated.h"

UINTERFACE(BlueprintType)
class OMNIS_API UOmnisInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

class OMNIS_API IOmnisInteractableInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Omnis|Interaction")
    void Interact(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Omnis|Interaction")
    FText GetInteractionPrompt() const;
};
