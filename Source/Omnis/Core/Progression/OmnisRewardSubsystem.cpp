#include "Core/Progression/OmnisRewardSubsystem.h"

#include "Core/Identity/OmnisAvatarSubsystem.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"

FOmnisRewardGrant UOmnisRewardSubsystem::GrantActivityReward(
    FName ActivityId,
    int32 ExperienceAmount,
    FName CosmeticId)
{
    LastReward = FOmnisRewardGrant();
    LastReward.RewardId = FName(*FString::Printf(TEXT("Reward.%d"), RewardCount + 1));
    LastReward.ActivityId = ActivityId;
    LastReward.Experience = FMath::Max(0, ExperienceAmount);
    LastReward.CosmeticId = CosmeticId;

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisProgressionSubsystem* Progression =
            GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
        {
            Progression->AddExperience(LastReward.Experience);
        }

        if (!CosmeticId.IsNone())
        {
            if (UOmnisAvatarSubsystem* Avatar =
                GameInstance->GetSubsystem<UOmnisAvatarSubsystem>())
            {
                Avatar->UnlockCosmetic(CosmeticId);
            }
        }
    }

    ++RewardCount;
    return LastReward;
}
