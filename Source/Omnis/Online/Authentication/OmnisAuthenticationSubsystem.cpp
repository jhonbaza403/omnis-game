#include "Online/Authentication/OmnisAuthenticationSubsystem.h"

void UOmnisAuthenticationSubsystem::SetExternalIdentity(FString NewUserId)
{
    NewUserId.TrimStartAndEndInline();
    if (NewUserId.IsEmpty())
    {
        SignOut();
        return;
    }

    UserId = MoveTemp(NewUserId);
    State = AccessToken.IsEmpty() ? EOmnisAuthState::LocalIdentity : EOmnisAuthState::SignedIn;
}

void UOmnisAuthenticationSubsystem::SetAccessToken(FString NewAccessToken)
{
    NewAccessToken.TrimStartAndEndInline();
    AccessToken = MoveTemp(NewAccessToken);

    if (UserId.IsEmpty())
    {
        State = EOmnisAuthState::SignedOut;
    }
    else
    {
        State = AccessToken.IsEmpty() ? EOmnisAuthState::LocalIdentity : EOmnisAuthState::SignedIn;
    }
}

void UOmnisAuthenticationSubsystem::SignOut()
{
    UserId.Reset();
    AccessToken.Reset();
    State = EOmnisAuthState::SignedOut;
}
