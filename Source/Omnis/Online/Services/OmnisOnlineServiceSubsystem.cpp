#include "Online/Services/OmnisOnlineServiceSubsystem.h"

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
