#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisQuestSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisQuestState : uint8
{
    Inactive,
    Active,
    Completed,
    Failed
};

UCLASS()
class OMNIS_API UOmnisQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Quest")
    void SetQuestState(FName QuestId, EOmnisQuestState State);

    UFUNCTION(BlueprintPure, Category="Omnis|Quest")
    EOmnisQuestState GetQuestState(FName QuestId) const;

private:
    UPROPERTY()
    TMap<FName, EOmnisQuestState> QuestStates;
};
