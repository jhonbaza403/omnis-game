#include "World/Economy/OmnisEconomySimulationSubsystem.h"
#include "Core/Time/OmnisTimeSubsystem.h"
#include "Engine/World.h"
#include "World/Economy/OmnisEconomySubsystem.h"

bool UOmnisEconomySimulationSubsystem::IsTickable() const
{
    const UWorld* World = GetWorld();
    return World && World->GetNetMode() != NM_Client;
}

void UOmnisEconomySimulationSubsystem::Tick(float DeltaSeconds)
{
    (void)DeltaSeconds;
    UWorld* World = GetWorld();
    if (!World || World->GetNetMode() == NM_Client)
    {
        return;
    }

    const UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>();
    UGameInstance* GameInstance = World->GetGameInstance();
    UOmnisEconomySubsystem* Economy = GameInstance
        ? GameInstance->GetSubsystem<UOmnisEconomySubsystem>()
        : nullptr;
    if (!Time || !Economy)
    {
        return;
    }

    const int64 CurrentGameDay = FMath::Max<int64>(
        0,
        FMath::FloorToDouble(Time->GetElapsedSeconds() / SecondsPerGameDay));

    // The first tick establishes the baseline; loading a save never simulates
    // an unbounded offline interval or duplicates the current in-game day.
    if (LastSimulatedGameDay == INDEX_NONE)
    {
        LastSimulatedGameDay = CurrentGameDay;
        return;
    }
    if (CurrentGameDay <= LastSimulatedGameDay)
    {
        return;
    }

    const int64 ElapsedDays = CurrentGameDay - LastSimulatedGameDay;
    const int32 DaysToCatchUp = static_cast<int32>(FMath::Min<int64>(ElapsedDays, 7));
    Economy->AdvanceAutonomousDay(DaysToCatchUp);
    LastSimulatedGameDay += DaysToCatchUp;

    if (ElapsedDays > 7)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("OMNIS economy catch-up capped at seven game days; remaining days will be processed on later ticks."));
    }
}
