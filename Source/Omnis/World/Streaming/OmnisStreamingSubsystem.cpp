#include "World/Streaming/OmnisStreamingSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

bool UOmnisStreamingSubsystem::RequestRegion(FName RegionId)
{
    if (RegionId.IsNone() || LoadedRegions.Contains(RegionId))
    {
        return false;
    }

    if (LoadedRegions.Num() >= MaxResidentRegions)
    {
        return false;
    }

    LoadedRegions.Add(RegionId);
    return true;
}

void UOmnisStreamingSubsystem::ReleaseRegion(FName RegionId)
{
    if (TSharedPtr<FStreamableHandle>* Handle = RegionHandles.Find(RegionId))
    {
        if (Handle->IsValid())
        {
            (*Handle)->ReleaseHandle();
        }
        RegionHandles.Remove(RegionId);
    }

    RegionAssets.Remove(RegionId);
    LoadedRegions.Remove(RegionId);
}

bool UOmnisStreamingSubsystem::RequestRegionAsset(FName RegionId, const FSoftObjectPath& AssetPath)
{
    if (RegionId.IsNone() || !AssetPath.IsValid())
    {
        return false;
    }

    if (!RequestRegion(RegionId))
    {
        return false;
    }

    RegionAssets.Add(RegionId, AssetPath);

    FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
    TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
        AssetPath,
        FStreamableDelegate::CreateUObject(this, &ThisClass::HandleAssetLoaded, RegionId)
    );

    if (!Handle.IsValid())
    {
        ReleaseRegion(RegionId);
        return false;
    }

    RegionHandles.Add(RegionId, Handle);
    return true;
}

void UOmnisStreamingSubsystem::ReleaseRegionAsset(FName RegionId)
{
    ReleaseRegion(RegionId);
}

void UOmnisStreamingSubsystem::SetMaxResidentRegions(int32 MaxRegions)
{
    MaxResidentRegions = FMath::Clamp(MaxRegions, 1, 64);

    while (LoadedRegions.Num() > MaxResidentRegions)
    {
        const FName RegionToRelease = LoadedRegions.Array()[0];
        ReleaseRegion(RegionToRelease);
    }
}

bool UOmnisStreamingSubsystem::IsRegionLoaded(FName RegionId) const
{
    return LoadedRegions.Contains(RegionId);
}

FOmnisStreamingStatus UOmnisStreamingSubsystem::GetStatus() const
{
    FOmnisStreamingStatus Status;
    Status.RequestedRegions = RegionAssets.Num();
    Status.LoadedRegions = LoadedRegions.Num();
    Status.MaxResidentRegions = MaxResidentRegions;
    return Status;
}

void UOmnisStreamingSubsystem::HandleAssetLoaded(FName RegionId)
{
    if (!RegionAssets.Contains(RegionId))
    {
        ReleaseRegion(RegionId);
        return;
    }
}
