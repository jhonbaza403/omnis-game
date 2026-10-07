#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisWildlifeActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class OMNIS_API AOmnisWildlifeActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisWildlifeActor();

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Wildlife")
    void ConfigureWildlifeAsset(const FString& MeshAssetPath);

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Visual;

private:
    void ApplyConfiguredMesh();

    float WanderTime = 0.0f;
    FString WildlifeMeshPath;
};
