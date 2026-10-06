#include "Core/Nexus/OmnisNexusSubsystem.h"

void UOmnisNexusSubsystem::AddPermanentUpgradePoints(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    Progression.PermanentUpgradePoints += Amount;
}
