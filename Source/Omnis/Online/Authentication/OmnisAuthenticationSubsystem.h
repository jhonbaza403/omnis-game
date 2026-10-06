#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAuthenticationSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisAuthState : uint8
{
    SignedOut,
    LocalIdentity,
    SignedIn
};

UCLASS()
class OMNIS_API UOmnisAuthenticationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetExternalIdentity(FString NewUserId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetAccessToken(FString NewAccessToken);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void RefreshPlatformIdentity();

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SignOut();

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    EOmnisAuthState GetState() const { return State; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetUserId() const { return UserId; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetPlatformUserId() const { return PlatformUserId; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetDisplayName() const { return DisplayName; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsActivePlatformUserValid() const { return bPlatformUserValid; }

    FString GetAccessToken() const { return AccessToken; }

private:
    void HandleLoginStatusChanged(
        int32 LocalUserNum,
        ELoginStatus::Type PreviousStatus,
        ELoginStatus::Type NewStatus,
        const FUniqueNetId& UniqueId
    );

    UPROPERTY()
    FString UserId;

    UPROPERTY()
    FString PlatformUserId;

    UPROPERTY()
    FString DisplayName;

    UPROPERTY(Transient)
    FString AccessToken;

    UPROPERTY()
    EOmnisAuthState State = EOmnisAuthState::SignedOut;

    UPROPERTY()
    bool bPlatformUserValid = false;

    FDelegateHandle LoginStatusDelegateHandle;
};
