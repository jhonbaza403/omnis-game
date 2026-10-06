#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisOnlineServiceSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisOnlineCapabilities
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bIdentity = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bSessions = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bPresence = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bFriends = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bStats = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bAchievements = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bEntitlements = false;

    UPROPERTY(BlueprintReadOnly, Category="Online")
    bool bTitleFile = false;
};

UCLASS()
class OMNIS_API UOmnisOnlineServiceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool RefreshCapabilities();

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FOmnisOnlineCapabilities GetCapabilities() const { return Capabilities; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsOnlineServiceReady() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetProviderName() const { return ProviderName; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetServiceAvailable(FName ServiceId, bool bAvailable);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsServiceAvailable(FName ServiceId) const;

private:
    UPROPERTY()
    TMap<FName, bool> Services;

    UPROPERTY()
    FOmnisOnlineCapabilities Capabilities;

    UPROPERTY()
    FString ProviderName;
};
