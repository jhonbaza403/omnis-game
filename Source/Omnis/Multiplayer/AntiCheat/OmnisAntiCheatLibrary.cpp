#include "Multiplayer/AntiCheat/OmnisAntiCheatLibrary.h"

bool UOmnisAntiCheatLibrary::IsFiniteFloat(float Value)
{
    return FMath::IsFinite(Value);
}

float UOmnisAntiCheatLibrary::ClampClientFloat(float Value, float MinValue, float MaxValue)
{
    if (!FMath::IsFinite(Value))
    {
        return MinValue;
    }

    return FMath::Clamp(Value, MinValue, MaxValue);
}
