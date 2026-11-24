#include "Core/Nexus/OmnisNexusSubsystem.h"

void UOmnisNexusSubsystem::AddPermanentUpgradePoints(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    Progression.PermanentUpgradePoints += Amount;
}

bool UOmnisNexusSubsystem::PurchaseVitalityUpgrade()
{
    if (Progression.PermanentUpgradePoints <= 0)
    {
        return false;
    }

    --Progression.PermanentUpgradePoints;
    ++Progression.PermanentVitalityLevel;
    return true;
}

void UOmnisNexusSubsystem::ImportProgression(
    const FOmnisNexusProgression& SavedProgression)
{
    Progression.PermanentUpgradePoints = FMath::Max(0, SavedProgression.PermanentUpgradePoints);
    Progression.PermanentVitalityLevel = FMath::Max(0, SavedProgression.PermanentVitalityLevel);
}
