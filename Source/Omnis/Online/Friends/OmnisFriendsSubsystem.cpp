#include "Online/Friends/OmnisFriendsSubsystem.h"

bool UOmnisFriendsSubsystem::AddFriend(FName UserId)
{
    return !UserId.IsNone() && Friends.Add(UserId).IsValidId();
}

void UOmnisFriendsSubsystem::RemoveFriend(FName UserId)
{
    Friends.Remove(UserId);
}

bool UOmnisFriendsSubsystem::IsFriend(FName UserId) const
{
    return Friends.Contains(UserId);
}
