#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisStreamingSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisStreamingStatus
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Streaming")
    int32 RequestedRegions = 0;

    UPROPERTY(BlueprintReadOnly, Category="Streaming")
    int32 LoadedRegions = 0;

    UPROPERTY(BlueprintReadOnly, Category="Streaming")
    int32 MaxResidentRegions = 8;
};

UCLASS()
class OMNIS_API UOmnisStreamingSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    bool RequestRegion(FName RegionId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void ReleaseRegion(FName RegionId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    bool RequestRegionAsset(FName RegionId, const FSoftObjectPath& AssetPath);

    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void ReleaseRegionAsset(FName RegionId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void SetMaxResidentRegions(int32 MaxRegions);

    UFUNCTION(BlueprintPure, Category="Omnis|Streaming")
    bool IsRegionLoaded(FName RegionId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Streaming")
    FOmnisStreamingStatus GetStatus() const;

private:
    void HandleAssetLoaded(FName RegionId);

    UPROPERTY()
    TSet<FName> LoadedRegions;

    UPROPERTY()
    TMap<FName, FSoftObjectPath> RegionAssets;

    TMap<FName, TSharedPtr<FStreamableHandle>> RegionHandles;

    UPROPERTY()
    int32 MaxResidentRegions = 8;
};
