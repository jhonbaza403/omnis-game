#include "Core/Events/OmnisEventSubsystem.h"

void UOmnisEventSubsystem::RaiseEvent(FName EventId, int32 Value)
{
    if (!EventId.IsNone())
    {
        OnEventRaised.Broadcast(EventId, Value);
    }
}
