#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Interaction/OmnisInteractableInterface.h"
#include "Net/UnrealNetwork.h"
#include "OmnisNPCCharacter.generated.h"

class UStaticMeshComponent;
class UOmnisHealthComponent;
class UOmnisProceduralAnimationComponent;
class UOmnisCharacterStateComponent;
class UOmnisPerceptionComponent;
class UOmnisCharacterDefinition;
struct FOmnisCharacterLifeSnapshot;

UCLASS()
class OMNIS_API AOmnisNPCCharacter : public ACharacter, public IOmnisInteractableInterface
{
    GENERATED_BODY()

public:
    AOmnisNPCCharacter();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;

    void RestorePersistentState(const FTransform& SavedTransform, const FOmnisCharacterLifeSnapshot& SavedLifeState);
    FName GetPersistentId() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Omnis|NPC")
    FName CharacterId = TEXT("NPC.Wanderer");

    // Assign a unique, durable ID to every placed and dynamically spawned NPC.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Omnis|NPC")
    FName PersistentId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    TSoftObjectPtr<UOmnisCharacterDefinition> CharacterDefinition;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|NPC")
    FText CharacterDisplayName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|NPC")
    FText CharacterBiography;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|NPC")
    FName OccupationId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|NPC")
    FName HomeLocationId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|NPC")
    FName WorkLocationId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|NPC")
    FText InteractionPrompt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life")
    FVector WorkLocationOffset = FVector(1200.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="1.0"))
    float LifeUpdateInterval = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float WorkStartHour = 7.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float WorkEndHour = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float RestStartHour = 21.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float RestEndHour = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="100.0"))
    float HungerThreshold = 65.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="100.0"))
    float EnergyThreshold = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="0.0", ClampMax="100.0"))
    float SocialThreshold = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="100.0", ClampMax="5000.0"))
    float ExploreRadius = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Life", meta=(ClampMin="100.0", ClampMax="5000.0"))
    float SocialRadius = 1800.0f;

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
    void ApplyCharacterDefinition();
    void SimulateLife(float DeltaSeconds);
    void SpreadRumorsTo(AActor* Target);
    void MoveToLifeDestination(const FVector& Destination, float AcceptanceRadius = 100.0f);

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Visual;

    FVector HomeLocation = FVector::ZeroVector;
    FVector WorkLocation = FVector::ZeroVector;
    FVector CurrentDestination = FVector::ZeroVector;
    float LifeUpdateClock = 0.0f;
    bool bHasLifeDestination = false;
};
