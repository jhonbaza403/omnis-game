#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OmnisAircraftPawn.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EOmnisAircraftType : uint8
{
    Commercial,
    Private,
    Helicopter
};

UCLASS()
class OMNIS_API AOmnisAircraftPawn : public APawn
{
    GENERATED_BODY()

public:
    AOmnisAircraftPawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Aircraft")
    void ConfigureAircraft(EOmnisAircraftType InType, const FString& MeshAssetPath);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> AircraftMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Omnis|Aircraft")
    EOmnisAircraftType AircraftType = EOmnisAircraftType::Commercial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Omnis|Aircraft")
    float ThrustForce = 2500000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Omnis|Aircraft")
    float ControlTorque = 7000000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Omnis|Aircraft")
    float LiftForce = 900000.0f;

private:
    void Throttle(float Value);
    void Pitch(float Value);
    void Roll(float Value);
    void Yaw(float Value);
    void ApplyConfiguredMesh();

    FString AircraftMeshPath;
    float ThrottleInput = 0.0f;
    float PitchInput = 0.0f;
    float RollInput = 0.0f;
    float YawInput = 0.0f;
};