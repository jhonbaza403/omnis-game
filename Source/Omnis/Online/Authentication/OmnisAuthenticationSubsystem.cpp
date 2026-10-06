#include "Online/Authentication/OmnisAuthenticationSubsystem.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"

void UOmnisAuthenticationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
        {
            LoginStatusDelegateHandle = Identity->AddOnLoginStatusChangedDelegate_Handle(
                0,
                FOnLoginStatusChangedDelegate::CreateUObject(this, &ThisClass::HandleLoginStatusChanged)
            );
        }
    }

    RefreshPlatformIdentity();
}

void UOmnisAuthenticationSubsystem::Deinitialize()
{
    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
        {
            if (LoginStatusDelegateHandle.IsValid())
            {
                Identity->ClearOnLoginStatusChangedDelegate_Handle(0, LoginStatusDelegateHandle);
            }
        }
    }

    Super::Deinitialize();
}

void UOmnisAuthenticationSubsystem::SetExternalIdentity(FString NewUserId)
{
    NewUserId.TrimStartAndEndInline();
    if (NewUserId.IsEmpty())
    {
        UserId.Reset();
        State = bPlatformUserValid ? EOmnisAuthState::SignedIn : EOmnisAuthState::SignedOut;
        return;
    }

    UserId = MoveTemp(NewUserId);
    State = AccessToken.IsEmpty() ? EOmnisAuthState::LocalIdentity : EOmnisAuthState::SignedIn;
}

void UOmnisAuthenticationSubsystem::SetAccessToken(FString NewAccessToken)
{
    NewAccessToken.TrimStartAndEndInline();
    AccessToken = MoveTemp(NewAccessToken);

    if (UserId.IsEmpty() && !bPlatformUserValid)
    {
        State = EOmnisAuthState::SignedOut;
    }
    else
    {
        State = AccessToken.IsEmpty() ? EOmnisAuthState::LocalIdentity : EOmnisAuthState::SignedIn;
    }
}

void UOmnisAuthenticationSubsystem::RefreshPlatformIdentity()
{
    PlatformUserId.Reset();
    DisplayName.Reset();
    bPlatformUserValid = false;

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return;
    }

    const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
    if (!Identity.IsValid() || Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        return;
    }

    const FUniqueNetIdPtr UniqueId = Identity->GetUniquePlayerId(0);
    if (!UniqueId.IsValid())
    {
        return;
    }

    PlatformUserId = UniqueId->ToString();
    DisplayName = Identity->GetPlayerNickname(*UniqueId);
    bPlatformUserValid = !PlatformUserId.IsEmpty();

    if (!UserId.IsEmpty() || !AccessToken.IsEmpty())
    {
        State = AccessToken.IsEmpty() ? EOmnisAuthState::LocalIdentity : EOmnisAuthState::SignedIn;
    }
    else
    {
        State = EOmnisAuthState::SignedIn;
    }
}

void UOmnisAuthenticationSubsystem::SignOut()
{
    UserId.Reset();
    AccessToken.Reset();
    PlatformUserId.Reset();
    DisplayName.Reset();
    bPlatformUserValid = false;
    State = EOmnisAuthState::SignedOut;
}

void UOmnisAuthenticationSubsystem::HandleLoginStatusChanged(
    int32 LocalUserNum,
    ELoginStatus::Type,
    ELoginStatus::Type,
    const FUniqueNetId&)
{
    if (LocalUserNum == 0)
    {
        RefreshPlatformIdentity();
    }
}
