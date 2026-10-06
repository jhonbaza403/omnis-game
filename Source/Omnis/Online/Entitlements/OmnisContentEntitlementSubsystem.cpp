#include "Online/Entitlements/OmnisContentEntitlementSubsystem.h"

#include "Interfaces/OnlineEntitlementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"

bool UOmnisContentEntitlementSubsystem::RefreshEntitlements(const FString& Namespace)
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
    const IOnlineEntitlementsPtr Entitlements = OnlineSubsystem->GetEntitlementsInterface();
    if (!Identity.IsValid() || !Entitlements.IsValid() ||
        Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    const FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
    if (!UserId.IsValid())
    {
        return false;
    }

    CachedEntitlementItemIds.Reset();

    TArray<TSharedRef<FOnlineEntitlement>> CurrentEntitlements;
    Entitlements->GetAllEntitlements(*UserId, Namespace, CurrentEntitlements);

    for (const TSharedRef<FOnlineEntitlement>& Entitlement : CurrentEntitlements)
    {
        if (!Entitlement->ItemId.IsEmpty() &&
            (Entitlement->Status.Equals(TEXT("Active"), ESearchCase::IgnoreCase) ||
             Entitlement->Status.Equals(TEXT("Subscribed"), ESearchCase::IgnoreCase)))
        {
            CachedEntitlementItemIds.Add(Entitlement->ItemId);
        }
    }

    return Entitlements->QueryEntitlements(*UserId, Namespace, FPagedQuery(0, -1));
}

bool UOmnisContentEntitlementSubsystem::IsContentEntitled(const FString& ItemId) const
{
    return !ItemId.IsEmpty() && CachedEntitlementItemIds.Contains(ItemId);
}

void UOmnisContentEntitlementSubsystem::ClearCachedEntitlements()
{
    CachedEntitlementItemIds.Reset();
}
