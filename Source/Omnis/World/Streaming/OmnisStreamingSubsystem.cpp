#include "World/Streaming/OmnisStreamingSubsystem.h"

void UOmnisStreamingSubsystem::RequestRegion(FName RegionId)
{
    if (!RegionId.IsNone())
    {
        LoadedRegions.Add(RegionId);
    }
}

void UOmnisStreamingSubsystem::ReleaseRegion(FName RegionId)
{
    LoadedRegions.Remove(RegionId);
}

bool UOmnisStreamingSubsystem::IsRegionLoaded(FName RegionId) const
{
    return LoadedRegions.Contains(RegionId);
}
