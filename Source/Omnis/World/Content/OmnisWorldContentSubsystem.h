#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisWorldContentSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisWorldContentSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|World|Content")
    void BuildProductionWorld();

    UFUNCTION(BlueprintPure, Category="Omnis|World|Content")
    bool IsProductionWorldBuilt() const { return bBuilt; }

private:
    AActor* SpawnStaticAsset(const FString& AssetPath, const FVector& Location, const FVector& Scale = FVector(1.0f));

    bool bBuilt = false;
};
