#include "UI/HUD/OmnisHUDStateSubsystem.h"

void UOmnisHUDStateSubsystem::PushNotification(FText Message)
{
    if (!Message.IsEmpty())
    {
        OnNotification.Broadcast(Message);
    }
}

void UOmnisHUDStateSubsystem::SetActiveUserDisplayName(FString Name)
{
    Name.TrimStartAndEndInline();
    ActiveUserDisplayName = FText::FromString(Name.Left(64));
    if (!ActiveUserDisplayName.IsEmpty())
    {
        OnActiveUserChanged.Broadcast(ActiveUserDisplayName);
    }
}
