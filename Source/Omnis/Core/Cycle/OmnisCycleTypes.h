#pragma once

#include "CoreMinimal.h"
#include "OmnisCycleTypes.generated.h"

UENUM(BlueprintType)
enum class EOmnisCyclePhase : uint8
{
    Awakening UMETA(DisplayName="Awakening"),
    Exploration UMETA(DisplayName="Exploration"),
    Crisis UMETA(DisplayName="Crisis"),
    Collapse UMETA(DisplayName="Collapse"),
    Nexus UMETA(DisplayName="Nexus"),
    Rebirth UMETA(DisplayName="Rebirth")
};

USTRUCT(BlueprintType)
struct FOmnisCycleState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cycle")
    int32 CycleIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cycle")
    EOmnisCyclePhase Phase = EOmnisCyclePhase::Awakening;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cycle")
    float WorldAge = 0.0f;
};
