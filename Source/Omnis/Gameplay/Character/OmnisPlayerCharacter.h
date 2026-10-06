#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OmnisPlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
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

UCLASS()
class OMNIS_API AOmnisPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOmnisPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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

protected:
    void MoveForwardInput(const FInputActionValue& Value);
    void MoveRightInput(const FInputActionValue& Value);
    void LookYawInput(const FInputActionValue& Value);
    void LookPitchInput(const FInputActionValue& Value);
    void JumpInput(const FInputActionValue& Value);
    void SprintInput(const FInputActionValue& Value);
    void AttackInput(const FInputActionValue& Value);
    void InteractInput(const FInputActionValue& Value);
    void BuildInput(const FInputActionValue& Value);
    void MenuInput(const FInputActionValue& Value);
    void BackInput(const FInputActionValue& Value);

    UFUNCTION()
    void HandlePlayerDeath();

private:
    void InitializeEnhancedInput();
    void RemoveEnhancedInput();

    UPROPERTY(Transient)
    TObjectPtr<UInputMappingContext> InputMappingContext;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MoveForwardAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MoveRightAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> LookYawAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> LookPitchAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> AttackAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> BuildAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MenuAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> BackAction;

    bool bEnhancedInputRegistered = false;
};
