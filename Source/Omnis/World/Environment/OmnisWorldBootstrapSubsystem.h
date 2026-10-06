#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisWorldBootstrapSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisWorldBootstrapSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|World")
    void BuildInitialWorld();

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    bool IsWorldBuilt() const { return bWorldBuilt; }

private:
    UPROPERTY()
    bool bWorldBuilt = false;
};
