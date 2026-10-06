#include "Core/Progression/OmnisProgressionSubsystem.h"

void UOmnisProgressionSubsystem::AddExperience(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    Progression.Experience += Amount;
    RecalculateLevel();
}

void UOmnisProgressionSubsystem::RecalculateLevel()
{
    constexpr int32 ExperiencePerLevel = 100;
    Progression.Level = FMath::Max(1, (Progression.Experience / ExperiencePerLevel) + 1);
}
