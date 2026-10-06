#include "Online/Services/OmnisOnlineServiceSubsystem.h"

#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineEntitlementsInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineTitleFileInterface.h"
#include "OnlineSubsystem.h"

void UOmnisOnlineServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    RefreshCapabilities();
}

void UOmnisOnlineServiceSubsystem::Deinitialize()
{
    Services.Reset();
    Capabilities = FOmnisOnlineCapabilities();
    ProviderName.Reset();
    Super::Deinitialize();
}

bool UOmnisOnlineServiceSubsystem::RefreshCapabilities()
{
    Capabilities = FOmnisOnlineCapabilities();
    ProviderName.Reset();

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    ProviderName = OnlineSubsystem->GetSubsystemName().ToString();
    Capabilities.bIdentity = OnlineSubsystem->GetIdentityInterface().IsValid();
    Capabilities.bSessions = OnlineSubsystem->GetSessionInterface().IsValid();
    Capabilities.bPresence = OnlineSubsystem->GetPresenceInterface().IsValid();
    Capabilities.bFriends = OnlineSubsystem->GetFriendsInterface().IsValid();
    Capabilities.bStats = OnlineSubsystem->GetStatsInterface().IsValid();
    Capabilities.bAchievements = OnlineSubsystem->GetAchievementsInterface().IsValid();
    Capabilities.bEntitlements = OnlineSubsystem->GetEntitlementsInterface().IsValid();
    Capabilities.bTitleFile = OnlineSubsystem->GetTitleFileInterface().IsValid();

    return Capabilities.bIdentity;
}

bool UOmnisOnlineServiceSubsystem::IsOnlineServiceReady() const
{
    return Capabilities.bIdentity && (Capabilities.bSessions || Capabilities.bPresence);
}

void UOmnisOnlineServiceSubsystem::SetServiceAvailable(FName ServiceId, bool bAvailable)
{
    if (!ServiceId.IsNone())
    {
        Services.Add(ServiceId, bAvailable);
    }
}

bool UOmnisOnlineServiceSubsystem::IsServiceAvailable(FName ServiceId) const
{
    return Services.FindRef(ServiceId);
}
