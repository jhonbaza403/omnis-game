#include "World/Streaming/OmnisStreamingSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

bool UOmnisStreamingSubsystem::RequestRegion(FName RegionId)
{
    if (RegionId.IsNone() || RequestedRegions.Contains(RegionId))
    {
        return false;
    }

    if (LoadedRegions.Num() + RequestedRegions.Num() >= MaxResidentRegions)
    {
        return false;
    }

    RequestedRegions.Add(RegionId);
    LoadedRegions.Add(RegionId);
    OnRegionStreamingChanged.Broadcast(RegionId, true);
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
    const bool bWasLoaded = LoadedRegions.Remove(RegionId) > 0;
    RequestedRegions.Remove(RegionId);

    if (bWasLoaded)
    {
        OnRegionStreamingChanged.Broadcast(RegionId, false);
    }
}

bool UOmnisStreamingSubsystem::RequestRegionAsset(FName RegionId, const FSoftObjectPath& AssetPath)
{
    if (RegionId.IsNone() || !AssetPath.IsValid())
    {
        return false;
    }

    if (RequestedRegions.Contains(RegionId))
    {
        return false;
    }

    if (LoadedRegions.Num() + RequestedRegions.Num() >= MaxResidentRegions)
    {
        return false;
    }

    RequestedRegions.Add(RegionId);
    RegionAssets.Add(RegionId, AssetPath);

    FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
    TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(
        AssetPath,
        FStreamableDelegate::CreateUObject(this, &ThisClass::HandleAssetLoaded, RegionId)
    );

    if (!Handle.IsValid())
    {
        RequestedRegions.Remove(RegionId);
        RegionAssets.Remove(RegionId);
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

bool UOmnisStreamingSubsystem::IsRegionRequested(FName RegionId) const
{
    return RequestedRegions.Contains(RegionId);
}

bool UOmnisStreamingSubsystem::IsRegionLoaded(FName RegionId) const
{
    return LoadedRegions.Contains(RegionId);
}

FOmnisStreamingStatus UOmnisStreamingSubsystem::GetStatus() const
{
    FOmnisStreamingStatus Status;
    Status.RequestedRegions = RequestedRegions.Num();
    Status.LoadedRegions = LoadedRegions.Num();
    Status.MaxResidentRegions = MaxResidentRegions;
    return Status;
}

void UOmnisStreamingSubsystem::HandleAssetLoaded(FName RegionId)
{
    if (!RequestedRegions.Contains(RegionId) || !RegionAssets.Contains(RegionId))
    {
        return;
    }

    const FSoftObjectPath AssetPath = RegionAssets.FindRef(RegionId);
    if (!AssetPath.ResolveObject())
    {
        ReleaseRegion(RegionId);
        return;
    }

    if (LoadedRegions.Num() >= MaxResidentRegions)
    {
        ReleaseRegion(RegionId);
        return;
    }

    LoadedRegions.Add(RegionId);
    OnRegionStreamingChanged.Broadcast(RegionId, true);
}
