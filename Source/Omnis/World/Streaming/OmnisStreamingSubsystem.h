#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisStreamingSubsystem.generated.h"

class AActor;

UCLASS()
class OMNIS_API UOmnisStreamingSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Creates one real World Partition streaming source at each catalogued
    // subregion anchor. Anchors are relative Unreal coordinates, not GPS data.
    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void RequestRegion(FName RegionId);

    // Disables and destroys the streaming-source actors created for a region.
    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void ReleaseRegion(FName RegionId);

    // True only when every registered source for the region reports that its
    // affected World Partition streaming operations have completed.
    UFUNCTION(BlueprintPure, Category="Omnis|Streaming")
    bool IsRegionStreamingComplete(FName RegionId) const;

    // Backward-compatible Blueprint API. This reports request completion,
    // not proof that every asset in a geographic region exists or is loaded.
    UFUNCTION(BlueprintPure, Category="Omnis|Streaming")
    bool IsRegionLoaded(FName RegionId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Streaming")
    int32 GetRegionStreamingSourceCount(FName RegionId) const;

private:
    bool LoadRegionAnchorCatalog();
    bool HasValidRegionSources(FName RegionId) const;
    void ReleaseAllStreamingSources();

    // Runtime-created provider owners are transient and are never saved as
    // authored world content.
    UPROPERTY(Transient)
    TMap<FName, TArray<TObjectPtr<AActor>>> RegionStreamingSourceActors;

    // The catalog defines relative Unreal-space anchors for each region.
    TMap<FName, TArray<FVector>> RegionAnchors;

    bool bCatalogLoaded = false;
};
