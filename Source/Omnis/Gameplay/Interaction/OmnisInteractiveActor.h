#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "OmnisInteractiveActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisInteractiveActor : public AActor, public IOmnisInteractableInterface
{
    GENERATED_BODY()

public:
    AOmnisInteractiveActor();

    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override { return Prompt; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    FText Prompt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    FName InteractionId;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Mesh;
};
