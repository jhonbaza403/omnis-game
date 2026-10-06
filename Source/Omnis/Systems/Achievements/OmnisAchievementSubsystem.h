#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAchievementSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisAchievementDefinition
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Achievement")
    FString Id;

    UPROPERTY(BlueprintReadOnly, Category="Achievement")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category="Achievement")
    FString Description;
};

UCLASS()
class OMNIS_API UOmnisAchievementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Achievements")
    bool ReportProgress(const FString& AchievementId, float ProgressPercent);

    UFUNCTION(BlueprintCallable, Category="Omnis|Achievements")
    bool Unlock(const FString& AchievementId);

    UFUNCTION(BlueprintPure, Category="Omnis|Achievements")
    bool HasDefinition(const FString& AchievementId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Achievements")
    TArray<FOmnisAchievementDefinition> GetDefinitions() const { return Definitions; }

private:
    bool LoadDefinitions();

    UPROPERTY()
    TArray<FOmnisAchievementDefinition> Definitions;
};
