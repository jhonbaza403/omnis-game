#include "Multiplayer/Siege/OmnisSiegeSubsystem.h"

bool UOmnisSiegeSubsystem::StartSiege(FName TerritoryId, FName AttackerId)
{
    if (State.bActive || TerritoryId.IsNone() || AttackerId.IsNone())
    {
        return false;
    }

    State.TerritoryId = TerritoryId;
    State.AttackerId = AttackerId;
    State.bActive = true;
    return true;
}

void UOmnisSiegeSubsystem::EndSiege()
{
    State = FOmnisSiegeState{};
}
