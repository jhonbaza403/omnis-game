#include "Online/Friends/OmnisFriendsSubsystem.h"

bool UOmnisFriendsSubsystem::AddFriend(FName UserId)
{
    if (UserId.IsNone() || Friends.Contains(UserId))
    {
        return false;
    }

    Friends.Add(UserId);
    return true;
}

void UOmnisFriendsSubsystem::RemoveFriend(FName UserId)
{
    Friends.Remove(UserId);
}

bool UOmnisFriendsSubsystem::IsFriend(FName UserId) const
{
    return Friends.Contains(UserId);
}
