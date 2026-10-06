#include "Multiplayer/Territory/OmnisTerritorySubsystem.h"

bool UOmnisTerritorySubsystem::ClaimTerritory(FName TerritoryId, FName OwnerId)
{
    if (TerritoryId.IsNone() || OwnerId.IsNone() || Owners.Contains(TerritoryId))
    {
        return false;
    }

    Owners.Add(TerritoryId, OwnerId);
    return true;
}

FName UOmnisTerritorySubsystem::GetOwner(FName TerritoryId) const
{
    return Owners.FindRef(TerritoryId);
}
