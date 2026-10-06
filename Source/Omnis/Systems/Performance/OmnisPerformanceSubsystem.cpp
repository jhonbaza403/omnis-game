#include "Systems/Performance/OmnisPerformanceSubsystem.h"

#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMemory.h"

bool UOmnisPerformanceSubsystem::ApplyPerformanceTier(EOmnisPerformanceTier Tier)
{
    if (!GEngine)
    {
        return false;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    if (!Settings)
    {
        return false;
    }

    Settings->SetOverallScalabilityLevel(static_cast<int32>(Tier));
    Settings->SetDynamicResolutionEnabled(Tier >= EOmnisPerformanceTier::High);
    Settings->SetFrameRateLimit(
        Tier == EOmnisPerformanceTier::Low ? 30.0f :
        Tier == EOmnisPerformanceTier::Medium ? 45.0f : 60.0f
    );
    Settings->ApplyNonResolutionSettings();
    Settings->SaveSettings();

    ApplyStreamingBudgetMB(GetStreamingPoolSizeForTier(Tier));
    CurrentTier = Tier;
    return true;
}

void UOmnisPerformanceSubsystem::SetTargetFrameRate(float FrameRate)
{
    if (!GEngine || !GEngine->GetGameUserSettings())
    {
        return;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    Settings->SetFrameRateLimit(FMath::Max(0.0f, FrameRate));
    Settings->ApplyNonResolutionSettings();
    Settings->SaveSettings();
}

void UOmnisPerformanceSubsystem::SetDynamicResolutionEnabled(bool bEnabled)
{
    if (!GEngine || !GEngine->GetGameUserSettings())
    {
        return;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    Settings->SetDynamicResolutionEnabled(bEnabled);
    Settings->ApplyNonResolutionSettings();
    Settings->SaveSettings();
}

void UOmnisPerformanceSubsystem::ApplyStreamingBudgetMB(int32 PoolSizeMB)
{
    const int32 SafePoolSize = FMath::Clamp(PoolSizeMB, 0, 16384);
    if (IConsoleVariable* PoolSize = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.PoolSize")))
    {
        PoolSize->Set(SafePoolSize, ECVF_SetByCode);
    }
}

FOmnisMemorySnapshot UOmnisPerformanceSubsystem::GetMemorySnapshot() const
{
    const FPlatformMemoryStats Stats = FPlatformMemory::GetStats();

    FOmnisMemorySnapshot Snapshot;
    Snapshot.TotalPhysicalBytes = static_cast<int64>(Stats.TotalPhysical);
    Snapshot.AvailablePhysicalBytes = static_cast<int64>(Stats.AvailablePhysical);
    Snapshot.UsedPhysicalBytes = static_cast<int64>(Stats.UsedPhysical);
    Snapshot.UsedRatio = Stats.TotalPhysical > 0
        ? static_cast<float>(static_cast<double>(Stats.UsedPhysical) / static_cast<double>(Stats.TotalPhysical))
        : 0.0f;

    return Snapshot;
}

int32 UOmnisPerformanceSubsystem::GetStreamingPoolSizeForTier(EOmnisPerformanceTier Tier) const
{
    switch (Tier)
    {
        case EOmnisPerformanceTier::Low:
            return 512;
        case EOmnisPerformanceTier::Medium:
            return 1024;
        case EOmnisPerformanceTier::High:
            return 2048;
        case EOmnisPerformanceTier::VeryHigh:
            return 3072;
        case EOmnisPerformanceTier::Ultra:
            return 4096;
        default:
            return 2048;
    }
}
