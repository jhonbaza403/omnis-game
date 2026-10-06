#include "Platform/OmnisPlatformSubsystem.h"

#include "Core/Save/OmnisSaveSubsystem.h"
#include "Diagnostics/OmnisTelemetrySubsystem.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/PlatformMemory.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/CoreDelegates.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "OnlineSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogOmnisPlatform, Log, All);

void UOmnisPlatformSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    DetectPlatform();
    RefreshCapabilities();

    FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &ThisClass::HandleWillDeactivate);
    FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &ThisClass::HandleReactivated);
    FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &ThisClass::HandleWillEnterBackground);
    FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &ThisClass::HandleEnteredForeground);
    FCoreDelegates::OnHandleSystemError.AddUObject(this, &ThisClass::HandleSystemError);
    FCoreDelegates::OnHandleSystemHang.AddUObject(this, &ThisClass::HandleSystemHang);
}

void UOmnisPlatformSubsystem::Deinitialize()
{
    FCoreDelegates::ApplicationWillDeactivateDelegate.RemoveAll(this);
    FCoreDelegates::ApplicationHasReactivatedDelegate.RemoveAll(this);
    FCoreDelegates::ApplicationWillEnterBackgroundDelegate.RemoveAll(this);
    FCoreDelegates::ApplicationHasEnteredForegroundDelegate.RemoveAll(this);
    FCoreDelegates::OnHandleSystemError.RemoveAll(this);
    FCoreDelegates::OnHandleSystemHang.RemoveAll(this);

    Super::Deinitialize();
}

void UOmnisPlatformSubsystem::DetectPlatform()
{
    PlatformName = FPlatformMisc::GetPlatformName();

    if (PlatformName.Contains(TEXT("PS5"), ESearchCase::IgnoreCase))
    {
        PlatformFamily = EOmnisPlatformFamily::PlayStation5;
    }
    else if (PlatformName.Contains(TEXT("Xbox"), ESearchCase::IgnoreCase) ||
             PlatformName.Contains(TEXT("XSX"), ESearchCase::IgnoreCase) ||
             PlatformName.Contains(TEXT("Scarlett"), ESearchCase::IgnoreCase))
    {
        PlatformFamily = EOmnisPlatformFamily::XboxSeries;
    }
    else if (PlatformName.Contains(TEXT("Switch"), ESearchCase::IgnoreCase))
    {
        PlatformFamily = EOmnisPlatformFamily::Switch2;
    }
    else
    {
        PlatformFamily = EOmnisPlatformFamily::PC;
    }
}

void UOmnisPlatformSubsystem::RefreshCapabilities()
{
    Capabilities = FOmnisPlatformCapabilities();

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        Capabilities.bOnline = OnlineSubsystem->GetIdentityInterface().IsValid();
        Capabilities.bAchievements = OnlineSubsystem->GetAchievementsInterface().IsValid();
        Capabilities.bDLCEntitlements = OnlineSubsystem->GetEntitlementsInterface().IsValid();
    }

    if (GEngine && GEngine->GetGameUserSettings())
    {
        Capabilities.bHDR = GEngine->GetGameUserSettings()->SupportsHDRDisplayOutput();
    }
}

bool UOmnisPlatformSubsystem::HasNativeOnlineSubsystem() const
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    return OnlineSubsystem && OnlineSubsystem->GetIdentityInterface().IsValid();
}

bool UOmnisPlatformSubsystem::UnlockAchievement(const FString& AchievementId, float Progress)
{
    if (AchievementId.TrimStartAndCopy().IsEmpty())
    {
        return false;
    }

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
    const IOnlineAchievementsPtr Achievements = OnlineSubsystem->GetAchievementsInterface();
    if (!Identity.IsValid() || !Achievements.IsValid() ||
        Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    const FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
    if (!UserId.IsValid())
    {
        return false;
    }

    const float SafeProgress = FMath::Clamp(Progress, 0.0f, 100.0f);
    FOnlineAchievementsWriteRef WriteObject = MakeShared<FOnlineAchievementsWrite, ESPMode::ThreadSafe>();
    WriteObject->SetFloatStat(AchievementId, SafeProgress);
    Achievements->WriteAchievements(*UserId, WriteObject, FOnAchievementsWrittenDelegate());
    return true;
}

bool UOmnisPlatformSubsystem::QueryAchievements()
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
    const IOnlineAchievementsPtr Achievements = OnlineSubsystem->GetAchievementsInterface();
    if (!Identity.IsValid() || !Achievements.IsValid() ||
        Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    const FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
    if (!UserId.IsValid())
    {
        return false;
    }

    Achievements->QueryAchievementDescriptions(*UserId, FOnQueryAchievementsCompleteDelegate());
    Achievements->QueryAchievements(*UserId, FOnQueryAchievementsCompleteDelegate());
    return true;
}

bool UOmnisPlatformSubsystem::ApplyHDR(bool bEnable, int32 DisplayNits)
{
    if (!GEngine)
    {
        return false;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    if (!Settings || (bEnable && !Settings->SupportsHDRDisplayOutput()))
    {
        return false;
    }

    Settings->EnableHDRDisplayOutput(bEnable, FMath::Clamp(DisplayNits, 100, 4000));
    Settings->ApplyResolutionSettings(false);
    Settings->SaveSettings();
    Capabilities.bHDR = Settings->SupportsHDRDisplayOutput();
    return true;
}

bool UOmnisPlatformSubsystem::ApplyDynamicResolution(bool bEnable)
{
    if (!GEngine || !GEngine->GetGameUserSettings())
    {
        return false;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    Settings->SetDynamicResolutionEnabled(bEnable);
    Settings->ApplySettings(false);
    return true;
}

bool UOmnisPlatformSubsystem::ApplyFrameRateLimit(float FrameRate)
{
    if (!GEngine || !GEngine->GetGameUserSettings())
    {
        return false;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    Settings->SetFrameRateLimit(FMath::Max(0.0f, FrameRate));
    Settings->ApplySettings(false);
    return true;
}

bool UOmnisPlatformSubsystem::ApplyOverallScalability(int32 QualityLevel)
{
    if (!GEngine || !GEngine->GetGameUserSettings())
    {
        return false;
    }

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    Settings->SetOverallScalabilityLevel(FMath::Clamp(QualityLevel, 0, 4));
    Settings->ApplyNonResolutionSettings();
    Settings->SaveSettings();
    return true;
}

uint64 UOmnisPlatformSubsystem::GetTotalPhysicalMemoryBytes() const
{
    return FPlatformMemory::GetStats().TotalPhysical;
}

uint64 UOmnisPlatformSubsystem::GetAvailablePhysicalMemoryBytes() const
{
    return FPlatformMemory::GetStats().AvailablePhysical;
}

uint64 UOmnisPlatformSubsystem::GetUsedPhysicalMemoryBytes() const
{
    return FPlatformMemory::GetStats().UsedPhysical;
}

void UOmnisPlatformSubsystem::SaveForLifecycleTransition()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisSaveSubsystem* Save = GameInstance->GetSubsystem<UOmnisSaveSubsystem>())
        {
            if (!Save->SaveToSlot(1))
            {
                UE_LOG(LogOmnisPlatform, Warning, TEXT("Lifecycle save failed for slot 1"));
            }
        }

        if (UOmnisTelemetrySubsystem* Telemetry = GameInstance->GetSubsystem<UOmnisTelemetrySubsystem>())
        {
            Telemetry->RecordLifecycleEvent(TEXT("saving_for_lifecycle"));
        }
    }
}

void UOmnisPlatformSubsystem::SetLifecycle(EOmnisApplicationLifecycle NewLifecycle)
{
    if (Lifecycle == NewLifecycle)
    {
        return;
    }

    Lifecycle = NewLifecycle;
    OnLifecycleChanged.Broadcast(Lifecycle);
}

void UOmnisPlatformSubsystem::HandleWillDeactivate()
{
    SaveForLifecycleTransition();
    SetLifecycle(EOmnisApplicationLifecycle::Deactivating);
}

void UOmnisPlatformSubsystem::HandleReactivated()
{
    SetLifecycle(EOmnisApplicationLifecycle::Active);
}

void UOmnisPlatformSubsystem::HandleWillEnterBackground()
{
    SaveForLifecycleTransition();
    SetLifecycle(EOmnisApplicationLifecycle::Background);
}

void UOmnisPlatformSubsystem::HandleEnteredForeground()
{
    SetLifecycle(EOmnisApplicationLifecycle::Foreground);
    SetLifecycle(EOmnisApplicationLifecycle::Active);
}

void UOmnisPlatformSubsystem::HandleSystemError()
{
    const FString Directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Crashes"));
    IFileManager::Get().MakeDirectory(*Directory, true);

    const FString Marker = FString::Printf(
        TEXT("{\"game\":\"Omnis: El Último Ciclo\",\"platform\":\"%s\",\"event\":\"system_error\"}"),
        *PlatformName
    );

    FFileHelper::SaveStringToFile(Marker, *FPaths::Combine(Directory, TEXT("last_system_error.json")));

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisTelemetrySubsystem* Telemetry = GameInstance->GetSubsystem<UOmnisTelemetrySubsystem>())
        {
            Telemetry->RecordCrashEvent(TEXT("system_error"));
        }
    }
}

void UOmnisPlatformSubsystem::HandleSystemHang()
{
    const FString Directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Crashes"));
    IFileManager::Get().MakeDirectory(*Directory, true);

    const FString Marker = FString::Printf(
        TEXT("{\"game\":\"Omnis: El Último Ciclo\",\"platform\":\"%s\",\"event\":\"system_hang\"}"),
        *PlatformName
    );

    FFileHelper::SaveStringToFile(Marker, *FPaths::Combine(Directory, TEXT("last_system_hang.json")));

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisTelemetrySubsystem* Telemetry = GameInstance->GetSubsystem<UOmnisTelemetrySubsystem>())
        {
            Telemetry->RecordCrashEvent(TEXT("system_hang"));
        }
    }
}
