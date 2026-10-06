#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisPlatformSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisPlatformFamily : uint8
{
    Unknown,
    PC,
    PlayStation5,
    XboxSeries,
    Switch2
};

UENUM(BlueprintType)
enum class EOmnisApplicationLifecycle : uint8
{
    Active,
    Deactivating,
    Background,
    Foreground
};

USTRUCT(BlueprintType)
struct FOmnisPlatformCapabilities
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bGamepad = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bOnline = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bAchievements = false;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bSuspendResume = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bDLCEntitlements = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bHDR = false;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bDynamicResolution = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bStreaming = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bCrashReporting = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bAccessibility = true;

    UPROPERTY(BlueprintReadOnly, Category="Platform")
    bool bLocalization = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOmnisLifecycleChanged, EOmnisApplicationLifecycle, Lifecycle);

UCLASS()
class OMNIS_API UOmnisPlatformSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintPure, Category="Omnis|Platform")
    EOmnisPlatformFamily GetPlatformFamily() const { return PlatformFamily; }

    UFUNCTION(BlueprintPure, Category="Omnis|Platform")
    FString GetPlatformName() const { return PlatformName; }

    UFUNCTION(BlueprintPure, Category="Omnis|Platform")
    FOmnisPlatformCapabilities GetCapabilities() const { return Capabilities; }

    UFUNCTION(BlueprintPure, Category="Omnis|Platform")
    EOmnisApplicationLifecycle GetApplicationLifecycle() const { return Lifecycle; }

    UFUNCTION(BlueprintPure, Category="Omnis|Platform|Online")
    bool HasNativeOnlineSubsystem() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Achievements")
    bool UnlockAchievement(const FString& AchievementId, float Progress = 100.0f);

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Achievements")
    bool QueryAchievements();

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Video")
    bool ApplyHDR(bool bEnable, int32 DisplayNits = 1000);

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Video")
    bool ApplyDynamicResolution(bool bEnable);

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Video")
    bool ApplyFrameRateLimit(float FrameRate);

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Video")
    bool ApplyOverallScalability(int32 QualityLevel);

    UFUNCTION(BlueprintPure, Category="Omnis|Platform|Memory")
    uint64 GetTotalPhysicalMemoryBytes() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Platform|Memory")
    uint64 GetAvailablePhysicalMemoryBytes() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Platform|Memory")
    uint64 GetUsedPhysicalMemoryBytes() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Platform|Recovery")
    void SaveForLifecycleTransition();

    UPROPERTY(BlueprintAssignable, Category="Omnis|Platform")
    FOmnisLifecycleChanged OnLifecycleChanged;

private:
    void DetectPlatform();
    void RefreshCapabilities();
    void SetLifecycle(EOmnisApplicationLifecycle NewLifecycle);

    void HandleWillDeactivate();
    void HandleReactivated();
    void HandleWillEnterBackground();
    void HandleEnteredForeground();
    void HandleSystemError();
    void HandleSystemHang();

    EOmnisPlatformFamily PlatformFamily = EOmnisPlatformFamily::Unknown;
    EOmnisApplicationLifecycle Lifecycle = EOmnisApplicationLifecycle::Active;
    FString PlatformName;
    FOmnisPlatformCapabilities Capabilities;
};
