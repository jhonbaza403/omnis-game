#include "World/WorldState/OmnisWorldStateSubsystem.h"

void UOmnisWorldStateSubsystem::SetState(FName Key, int32 Value)
{
    if (!Key.IsNone())
    {
        Values.Add(Key, Value);
    }
}

int32 UOmnisWorldStateSubsystem::GetState(FName Key, int32 DefaultValue) const
{
    const int32* Value = Values.Find(Key);
    return Value ? *Value : DefaultValue;
}

void UOmnisWorldStateSubsystem::IncrementState(FName Key, int32 Delta)
{
    if (!Key.IsNone())
    {
        Values.FindOrAdd(Key) += Delta;
    }
}

void UOmnisWorldStateSubsystem::ImportStates(const TMap<FName, int32>& NewValues)
{
    Values.Reset();
    for (const TPair<FName, int32>& Pair : NewValues)
    {
        if (!Pair.Key.IsNone())
        {
            Values.Add(Pair.Key, Pair.Value);
        }
    }
}
