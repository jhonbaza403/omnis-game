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

void UOmnisProgressionSubsystem::RestoreProgression(int32 SavedExperience, int32 SavedLevel)
{
    Progression.Experience = FMath::Max(0, SavedExperience);
    Progression.Level = FMath::Max(1, SavedLevel);
    RecalculateLevel();
}

void UOmnisProgressionSubsystem::RecalculateLevel()
{
    constexpr int32 ExperiencePerLevel = 100;
    Progression.Level = FMath::Max(1, (Progression.Experience / ExperiencePerLevel) + 1);
}
