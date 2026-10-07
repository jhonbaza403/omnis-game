#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OmnisVehiclePawn.generated.h"

class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisVehiclePawn : public APawn
{
    GENERATED_BODY()

public:
    AOmnisVehiclePawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Vehicle")
    void ConfigureVehicle(const FString& MeshAssetPath);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> VehicleMesh;

private:
    void Throttle(float Value);
    void Steer(float Value);
    void BrakePressed();
    void BrakeReleased();
    void ApplyConfiguredMesh();

    UPROPERTY(EditAnywhere, Category="Omnis|Vehicle")
    float EngineForce = 1800000.0f;

    UPROPERTY(EditAnywhere, Category="Omnis|Vehicle")
    float SteeringTorque = 9000000.0f;

    UPROPERTY(EditAnywhere, Category="Omnis|Vehicle")
    float BrakeForce = 2400000.0f;

    FString VehicleMeshPath;
    float ThrottleInput = 0.0f;
    float SteerInput = 0.0f;
    bool bBrake = false;
};