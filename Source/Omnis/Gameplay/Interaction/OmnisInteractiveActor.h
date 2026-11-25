#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "Net/UnrealNetwork.h"
#include "OmnisInteractiveActor.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOmnisWorldInteractionEvent,
    AActor*, Interactor,
    FName, InteractionId,
    bool, bActivated);

UCLASS()
class OMNIS_API AOmnisInteractiveActor : public AActor, public IOmnisInteractableInterface
{
    GENERATED_BODY()

public:
    AOmnisInteractiveActor();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override { return Prompt; }

    UPROPERTY(BlueprintAssignable, Category="Omnis|Interaction")
    FOmnisWorldInteractionEvent OnInteractionActivated;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_InteractionState, Category="Omnis|Interaction")
    bool bActivated = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|Interaction")
    bool bConsumed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    FText Prompt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    FName InteractionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction", meta=(ClampMin="0.1", ClampMax="30.0"))
    float InteractionCooldownSeconds = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    bool bOneShot = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    int32 RewardExperience = 25;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    FName RewardFactionId = TEXT("Wanderers");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Interaction")
    int32 RewardFactionStanding = 5;

protected:
    UFUNCTION()
    void OnRep_InteractionState();

private:
    bool bRewardClaimed = false;
    double LastInteractionTimeSeconds = -100000.0;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Mesh;
};
