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
    State = EOmnisAuthState::SignedIn;
}

void UOmnisAuthenticationSubsystem::SignOut()
{
    UserId.Reset();
    State = EOmnisAuthState::SignedOut;
}
