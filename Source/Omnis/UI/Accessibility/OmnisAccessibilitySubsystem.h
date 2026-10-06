#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAccessibilitySubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisAccessibilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetSubtitleScale(float Scale);

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    float GetSubtitleScale() const { return SubtitleScale; }

private:
    UPROPERTY()
    float SubtitleScale = 1.0f;
};
