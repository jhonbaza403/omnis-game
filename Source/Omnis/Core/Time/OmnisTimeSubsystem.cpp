#include "Core/Time/OmnisTimeSubsystem.h"

void UOmnisTimeSubsystem::Tick(float DeltaSeconds)
{
    ElapsedSeconds += FMath::Max(0.0f, DeltaSeconds) * TimeScale;
}

void UOmnisTimeSubsystem::SetTimeScale(float NewScale)
{
    TimeScale = FMath::Clamp(NewScale, 0.0f, 8.0f);
}
