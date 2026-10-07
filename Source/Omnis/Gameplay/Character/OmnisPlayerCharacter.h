#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
class UOmnisProceduralAnimationComponent;
class UInputComponent;

UCLASS()
class OMNIS_API AOmnisPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOmnisPlayerCharacter();

    virtual void BeginPlay() override;
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Animation")
    TObjectPtr<UOmnisProceduralAnimationComponent> ProceduralAnimation;

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
};
