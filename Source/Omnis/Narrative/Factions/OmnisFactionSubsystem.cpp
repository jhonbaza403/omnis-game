#include "Narrative/Factions/OmnisFactionSubsystem.h"

void UOmnisFactionSubsystem::AdjustStanding(FName FactionId, int32 Delta)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    if (FactionId.IsNone() || Delta == 0)
    {
        return;
    }

    Standings.FindOrAdd(FactionId) = FMath::Clamp(
        Standings.FindRef(FactionId) + Delta,
        -100,
        100);
}

int32 UOmnisFactionSubsystem::GetStanding(FName FactionId) const
{
    return FMath::Clamp(Standings.FindRef(FactionId), -100, 100);
}

void UOmnisFactionSubsystem::ImportStandings(const TMap<FName, int32>& Values)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    Standings.Reset();
    for (const TPair<FName, int32>& Pair : Values)
    {
        if (!Pair.Key.IsNone())
        {
            Standings.Add(Pair.Key, FMath::Clamp(Pair.Value, -100, 100));
        }
    }
}
