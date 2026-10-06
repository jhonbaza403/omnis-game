#include "UI/Accessibility/OmnisAccessibilitySubsystem.h"

void UOmnisAccessibilitySubsystem::SetSubtitleScale(float Scale)
{
    SubtitleScale = FMath::Clamp(Scale, 0.75f, 2.5f);
}
