#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisStreamingSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisStreamingSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void RequestRegion(FName RegionId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Streaming")
    void ReleaseRegion(FName RegionId);

    UFUNCTION(BlueprintPure, Category="Omnis|Streaming")
    bool IsRegionLoaded(FName RegionId) const;

private:
    UPROPERTY()
    TSet<FName> LoadedRegions;
};
