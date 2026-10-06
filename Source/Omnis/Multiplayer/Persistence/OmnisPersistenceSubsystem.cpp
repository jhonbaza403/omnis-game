#include "Multiplayer/Persistence/OmnisPersistenceSubsystem.h"

void UOmnisPersistenceSubsystem::SetPersistentInt(FName Key, int32 Value)
{
    if (!Key.IsNone())
    {
        PersistentInts.Add(Key, Value);
    }
}

int32 UOmnisPersistenceSubsystem::GetPersistentInt(FName Key, int32 DefaultValue) const
{
    const int32* Value = PersistentInts.Find(Key);
    return Value ? *Value : DefaultValue;
}
