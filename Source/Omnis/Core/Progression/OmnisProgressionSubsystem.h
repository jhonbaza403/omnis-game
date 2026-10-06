#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisProgressionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisPlayerProgression
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Progression")
    int32 Experience = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Progression")
    int32 Level = 1;
};

UCLASS()
class OMNIS_API UOmnisProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Progression")
    void AddExperience(int32 Amount);

    UFUNCTION(BlueprintPure, Category="Omnis|Progression")
    FOmnisPlayerProgression GetProgression() const { return Progression; }

private:
    void RecalculateLevel();

    UPROPERTY()
    FOmnisPlayerProgression Progression;
};
