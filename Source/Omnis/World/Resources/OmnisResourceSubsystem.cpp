#include "World/Resources/OmnisResourceSubsystem.h"

void UOmnisResourceSubsystem::AddResource(FName ResourceId, int32 Amount)
{
    if (!ResourceId.IsNone() && Amount > 0)
    {
        Resources.FindOrAdd(ResourceId) += Amount;
    }
}

bool UOmnisResourceSubsystem::ConsumeResource(FName ResourceId, int32 Amount)
{
    if (Amount <= 0)
    {
        return false;
    }

    int32* Current = Resources.Find(ResourceId);
    if (!Current || *Current < Amount)
    {
        return false;
    }

    *Current -= Amount;
    return true;
}

int32 UOmnisResourceSubsystem::GetResource(FName ResourceId) const
{
    return Resources.FindRef(ResourceId);
}
