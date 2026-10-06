#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAccessibilitySubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisSubtitleMode : uint8
{
    Off,
    DialogueOnly,
    AllSpokenContent
};

UCLASS()
class OMNIS_API UOmnisAccessibilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetSubtitleScale(float Scale);

    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetSubtitleMode(EOmnisSubtitleMode NewMode);

    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetHighContrastUI(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetReduceMotion(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetColorBlindAssist(bool bEnabled);

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    float GetSubtitleScale() const { return SubtitleScale; }

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    EOmnisSubtitleMode GetSubtitleMode() const { return SubtitleMode; }

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    bool IsHighContrastUIEnabled() const { return bHighContrastUI; }

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    bool IsReduceMotionEnabled() const { return bReduceMotion; }

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    bool IsColorBlindAssistEnabled() const { return bColorBlindAssist; }

private:
    UPROPERTY()
    float SubtitleScale = 1.0f;

    UPROPERTY()
    EOmnisSubtitleMode SubtitleMode = EOmnisSubtitleMode::AllSpokenContent;

    UPROPERTY()
    bool bHighContrastUI = false;

    UPROPERTY()
    bool bReduceMotion = false;

    UPROPERTY()
    bool bColorBlindAssist = false;
};
