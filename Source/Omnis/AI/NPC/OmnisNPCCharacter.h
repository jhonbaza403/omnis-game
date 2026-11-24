#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "OmnisNPCCharacter.generated.h"

class UStaticMeshComponent;
class UOmnisHealthComponent;
class UOmnisProceduralAnimationComponent;
class UOmnisCharacterStateComponent;
class UOmnisPerceptionComponent;
struct FOmnisCharacterLifeSnapshot;

UCLASS()
class OMNIS_API AOmnisNPCCharacter : public ACharacter, public IOmnisInteractableInterface
{
    GENERATED_BODY()

public:
    AOmnisNPCCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;

    void RestorePersistentState(const FTransform& SavedTransform, const FOmnisCharacterLifeSnapshot& SavedLifeState);
    FName GetPersistentId() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    FName CharacterId = TEXT("NPC.Wanderer");

    // Assign explicitly to dynamically spawned NPCs; placed NPCs use a stable actor path fallback.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    FName PersistentId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    FText InteractionPrompt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life")
    FVector WorkLocationOffset = FVector(1200.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="1.0"))
    float LifeUpdateInterval = 3.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|NPC")
    TObjectPtr<UOmnisHealthComponent> Health;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    TObjectPtr<UOmnisCharacterStateComponent> CharacterState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    TObjectPtr<UOmnisPerceptionComponent> Perception;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Animation")
    TObjectPtr<UOmnisProceduralAnimationComponent> ProceduralAnimation;

protected:
    UFUNCTION()
    void HandleNPCDeath();

private:
    void SimulateLife(float DeltaSeconds);
    void MoveToLifeDestination(const FVector& Destination, float AcceptanceRadius = 100.0f);

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Visual;

    FVector HomeLocation = FVector::ZeroVector;
    FVector WorkLocation = FVector::ZeroVector;
    FVector CurrentDestination = FVector::ZeroVector;
    float LifeUpdateClock = 0.0f;
    bool bHasLifeDestination = false;
};
