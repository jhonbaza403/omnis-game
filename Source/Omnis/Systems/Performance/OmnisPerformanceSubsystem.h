#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisPerformanceSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisPerformanceTier : uint8
{
    Low = 0,
    Medium = 1,
    High = 2,
    VeryHigh = 3,
    Ultra = 4
};

USTRUCT(BlueprintType)
struct FOmnisMemorySnapshot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Memory")
    int64 TotalPhysicalBytes = 0;

    UPROPERTY(BlueprintReadOnly, Category="Memory")
    int64 AvailablePhysicalBytes = 0;

    UPROPERTY(BlueprintReadOnly, Category="Memory")
    int64 UsedPhysicalBytes = 0;

    UPROPERTY(BlueprintReadOnly, Category="Memory")
    float UsedRatio = 0.0f;
};

UCLASS()
class OMNIS_API UOmnisPerformanceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Performance")
    bool ApplyPerformanceTier(EOmnisPerformanceTier Tier);

    UFUNCTION(BlueprintCallable, Category="Omnis|Performance")
    void SetTargetFrameRate(float FrameRate);

    UFUNCTION(BlueprintCallable, Category="Omnis|Performance")
    void SetDynamicResolutionEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category="Omnis|Performance")
    void ApplyStreamingBudgetMB(int32 PoolSizeMB);

    UFUNCTION(BlueprintCallable, Category="Omnis|Performance")
    bool ApplyAdaptiveMemoryProtection(float WarningRatio = 0.85f, float CriticalRatio = 0.92f);

    UFUNCTION(BlueprintPure, Category="Omnis|Performance")
    FOmnisMemorySnapshot GetMemorySnapshot() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Performance")
    EOmnisPerformanceTier GetCurrentTier() const { return CurrentTier; }

private:
    int32 GetStreamingPoolSizeForTier(EOmnisPerformanceTier Tier) const;

    EOmnisPerformanceTier CurrentTier = EOmnisPerformanceTier::High;
};
