#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "OmnisPlayerCharacter.generated.h"

class UOmnisPlayerCameraComponent;
class USpringArmComponent;
class UOmnisExplorationComponent;
class UOmnisTraversalComponent;
class UOmnisInteractionComponent;
class UOmnisConstructionComponent;
class UOmnisCombatComponent;
class UOmnisAbilityComponent;
class UOmnisInventoryComponent;
class UOmnisEquipmentComponent;
class UOmnisHealthComponent;
class UOmnisCharacterStateComponent;
class UOmnisProceduralAnimationComponent;
class UStaticMeshComponent;
class UInputComponent;

UCLASS()
class OMNIS_API AOmnisPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOmnisPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Camera")
    TObjectPtr<UOmnisPlayerCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisExplorationComponent> Exploration;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisTraversalComponent> Traversal;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisInteractionComponent> Interaction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisConstructionComponent> Construction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisCombatComponent> Combat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisAbilityComponent> Abilities;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisInventoryComponent> Inventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisEquipmentComponent> Equipment;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Gameplay")
    TObjectPtr<UOmnisHealthComponent> Health;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    TObjectPtr<UOmnisCharacterStateComponent> CharacterState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Omnis|Life")
    bool bResting = false;

    UFUNCTION(BlueprintCallable, Category="Omnis|Survival")
    bool ConsumeFood(int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Survival")
    void SetResting(bool bNewResting);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Animation")
    TObjectPtr<UOmnisProceduralAnimationComponent> ProceduralAnimation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Avatar")
    TObjectPtr<UStaticMeshComponent> BodyMesh;

protected:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void SprintPressed();
    void SprintReleased();
    void AttackPressed();
    void InteractPressed();
    void BuildPressed();

    UFUNCTION()
    void HandlePlayerDeath();

    UFUNCTION(Server, Reliable)
    void ServerConsumeFood(int32 Quantity);

    UFUNCTION(Server, Reliable)
    void ServerSetResting(bool bNewResting);

private:
    void ApplyAvatarVisuals();
    bool ConsumeFoodOnAuthority(int32 Quantity);
    void SetRestingOnAuthority(bool bNewResting);
};
