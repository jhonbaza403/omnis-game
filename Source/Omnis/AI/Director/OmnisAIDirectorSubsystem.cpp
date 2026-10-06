#include "AI/Director/OmnisAIDirectorSubsystem.h"

void UOmnisAIDirectorSubsystem::SetThreatLevel(float Level)
{
    ThreatLevel = FMath::Clamp(Level, 0.0f, 1.0f);
}
