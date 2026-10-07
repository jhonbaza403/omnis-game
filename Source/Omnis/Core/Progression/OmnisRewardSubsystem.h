#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisRewardSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisRewardGrant
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Reward")
    FName RewardId;

    UPROPERTY(BlueprintReadOnly, Category="Reward")
    FName ActivityId;

    UPROPERTY(BlueprintReadOnly, Category="Reward")
    int32 Experience = 0;

    UPROPERTY(BlueprintReadOnly, Category="Reward")
    FName CosmeticId;
};

UCLASS()
class OMNIS_API UOmnisRewardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Rewards")
    FOmnisRewardGrant GrantActivityReward(FName ActivityId, int32 ExperienceAmount, FName CosmeticId = NAME_None);

    UFUNCTION(BlueprintPure, Category="Omnis|Rewards")
    FOmnisRewardGrant GetLastReward() const { return LastReward; }

    UFUNCTION(BlueprintPure, Category="Omnis|Rewards")
    int32 GetRewardCount() const { return RewardCount; }

private:
    UPROPERTY()
    FOmnisRewardGrant LastReward;

    UPROPERTY()
    int32 RewardCount = 0;
};
