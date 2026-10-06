#include "Developer/Profiling/OmnisProfilingSubsystem.h"
#include "HAL/PlatformTime.h"

void UOmnisProfilingSubsystem::StartMarker(FName MarkerId)
{
    if (!MarkerId.IsNone())
    {
        Started.Add(MarkerId, FPlatformTime::Seconds());
    }
}

void UOmnisProfilingSubsystem::EndMarker(FName MarkerId)
{
    if (const double* Start = Started.Find(MarkerId))
    {
        Durations.Add(MarkerId, static_cast<float>(FPlatformTime::Seconds() - *Start));
        Started.Remove(MarkerId);
    }
}

float UOmnisProfilingSubsystem::GetLastDuration(FName MarkerId) const
{
    return Durations.FindRef(MarkerId);
}
