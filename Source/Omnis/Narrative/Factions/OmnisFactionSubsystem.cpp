#include "Narrative/Factions/OmnisFactionSubsystem.h"

void UOmnisFactionSubsystem::AdjustStanding(FName FactionId, int32 Delta)
{
    if (FactionId.IsNone() || Delta == 0)
    {
        return;
    }

    Standings.FindOrAdd(FactionId) = FMath::Clamp(Standings.FindRef(FactionId) + Delta, -100, 100);
}

int32 UOmnisFactionSubsystem::GetStanding(FName FactionId) const
{
    return FMath::Clamp(Standings.FindRef(FactionId), -100, 100);
}
