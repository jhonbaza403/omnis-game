#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisExplorationComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisExplorationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisExplorationComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Exploration")
    int32 Scan();

    UFUNCTION(BlueprintPure, Category="Omnis|Exploration")
    const TArray<FName>& GetDiscoveries() const { return Discoveries; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Exploration")
    float ScanRadius = 1400.0f;

private:
    UPROPERTY()
    TArray<FName> Discoveries;
};
