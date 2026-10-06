#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisTraversalComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisTraversalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisTraversalComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Traversal")
    bool TryVault();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Omnis|Traversal")
    float VaultLaunchStrength = 520.0f;
};
