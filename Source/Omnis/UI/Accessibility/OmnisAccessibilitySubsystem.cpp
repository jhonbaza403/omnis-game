#include "UI/Accessibility/OmnisAccessibilitySubsystem.h"

void UOmnisAccessibilitySubsystem::SetSubtitleScale(float Scale)
{
    SubtitleScale = FMath::Clamp(Scale, 0.75f, 2.5f);
}

void UOmnisAccessibilitySubsystem::SetSubtitleMode(EOmnisSubtitleMode NewMode)
{
    SubtitleMode = NewMode;
}

void UOmnisAccessibilitySubsystem::SetHighContrastUI(bool bEnabled)
{
    bHighContrastUI = bEnabled;
}

void UOmnisAccessibilitySubsystem::SetReduceMotion(bool bEnabled)
{
    bReduceMotion = bEnabled;
}

void UOmnisAccessibilitySubsystem::SetColorBlindAssist(bool bEnabled)
{
    bColorBlindAssist = bEnabled;
}
