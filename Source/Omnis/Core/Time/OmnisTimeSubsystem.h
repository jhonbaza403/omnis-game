#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisTimeSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisWorldSeason : uint8
{
    Spring,
    Summer,
    Autumn,
    Winter
};

USTRUCT(BlueprintType)
struct FOmnisWorldCalendarDate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    int32 Year = 1;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    EOmnisWorldSeason Season = EOmnisWorldSeason::Spring;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    int32 DayOfYear = 1;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    int32 DayOfSeason = 1;

    // 0 = Monday, 6 = Sunday.
    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    int32 DayOfWeek = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    int32 Hour = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Time")
    int32 Minute = 0;
};

UCLASS()
class OMNIS_API UOmnisTimeSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaSeconds) override;
    virtual bool IsTickable() const override { return true; }
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UOmnisTimeSubsystem, STATGROUP_Tickables);
    }

    UFUNCTION(BlueprintCallable, Category="Omnis|Time")
    void SetTimeScale(float NewScale);

    UFUNCTION(BlueprintPure, Category="Omnis|Time")
    float GetElapsedSeconds() const { return ElapsedSeconds; }

    UFUNCTION(BlueprintPure, Category="Omnis|Time")
    FOmnisWorldCalendarDate GetCalendarDate() const;

    static FOmnisWorldCalendarDate BuildCalendarDate(float ElapsedGameSeconds);

    UFUNCTION(BlueprintCallable, Category="Omnis|Time")
    void RestoreElapsedSeconds(float NewElapsedSeconds);

    UFUNCTION(BlueprintPure, Category="Omnis|Time")
    float GetTimeScale() const { return TimeScale; }

private:
    UPROPERTY()
    float ElapsedSeconds = 0.0f;

    UPROPERTY()
    float TimeScale = 1.0f;
};
