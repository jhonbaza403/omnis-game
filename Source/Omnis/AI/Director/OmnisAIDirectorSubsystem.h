#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAIDirectorSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisAIDirectorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|AI")
    void SetThreatLevel(float Level);

    UFUNCTION(BlueprintPure, Category="Omnis|AI")
    float GetThreatLevel() const { return ThreatLevel; }

private:
    UPROPERTY()
    float ThreatLevel = 0.0f;
};
