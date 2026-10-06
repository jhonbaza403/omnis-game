#include "UI/HUD/OmnisHUDStateSubsystem.h"

void UOmnisHUDStateSubsystem::PushNotification(FText Message)
{
    if (!Message.IsEmpty())
    {
        OnNotification.Broadcast(Message);
    }
}
