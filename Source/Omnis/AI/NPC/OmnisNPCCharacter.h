#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "OmnisNPCCharacter.generated.h"

class UStaticMeshComponent;
class UOmnisHealthComponent;

UCLASS()
class OMNIS_API AOmnisNPCCharacter : public ACharacter, public IOmnisInteractableInterface
{
    GENERATED_BODY()

public:
    AOmnisNPCCharacter();

    virtual void Tick(float DeltaSeconds) override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override { return InteractionPrompt; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    FName CharacterId = TEXT("NPC.Wanderer");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    FText InteractionPrompt;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|NPC")
    TObjectPtr<UOmnisHealthComponent> Health;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Visual;

    float RoutineClock = 0.0f;
    FVector RoutineOrigin = FVector::ZeroVector;
};
