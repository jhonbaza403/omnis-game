#include "Core/Time/OmnisTimeSubsystem.h"

void UOmnisTimeSubsystem::Tick(float DeltaSeconds)
{
    ElapsedSeconds += FMath::Max(0.0f, DeltaSeconds) * TimeScale;
}

void UOmnisTimeSubsystem::SetTimeScale(float NewScale)
{
    TimeScale = FMath::Clamp(NewScale, 0.0f, 8.0f);
}

void UOmnisTimeSubsystem::RestoreElapsedSeconds(float NewElapsedSeconds)
{
    ElapsedSeconds = FMath::Max(
        0.0f,
        FMath::IsFinite(NewElapsedSeconds) ? NewElapsedSeconds : 0.0f);
}

FOmnisWorldCalendarDate UOmnisTimeSubsystem::GetCalendarDate() const
{
    return BuildCalendarDate(ElapsedSeconds);
}

FOmnisWorldCalendarDate UOmnisTimeSubsystem::BuildCalendarDate(float ElapsedGameSeconds)
{
    constexpr int64 MinutesPerHour = 60;
    constexpr int64 HoursPerDay = 24;
    constexpr int64 MinutesPerDay = MinutesPerHour * HoursPerDay;
    constexpr int64 DaysPerSeason = 30;
    constexpr int64 DaysPerYear = DaysPerSeason * 4;

    // The existing simulation rate is one in-game minute per real-time second
    // at time scale 1, which produces a 24-minute in-game day.
    const double SafeElapsed = FMath::Clamp(
        static_cast<double>(ElapsedGameSeconds),
        0.0,
        1000000000000.0);
    const int64 TotalMinutes = static_cast<int64>(SafeElapsed);
    const int64 TotalDays = TotalMinutes / MinutesPerDay;
    const int64 MinuteOfDay = TotalMinutes % MinutesPerDay;
    const int64 DayOfYearZeroBased = TotalDays % DaysPerYear;
    const int64 SeasonIndex = DayOfYearZeroBased / DaysPerSeason;

    FOmnisWorldCalendarDate Date;
    Date.Year = static_cast<int32>(TotalDays / DaysPerYear) + 1;
    Date.Season = static_cast<EOmnisWorldSeason>(SeasonIndex);
    Date.DayOfYear = static_cast<int32>(DayOfYearZeroBased) + 1;
    Date.DayOfSeason = static_cast<int32>(DayOfYearZeroBased % DaysPerSeason) + 1;
    Date.DayOfWeek = static_cast<int32>(TotalDays % 7);
    Date.Hour = static_cast<int32>(MinuteOfDay / MinutesPerHour);
    Date.Minute = static_cast<int32>(MinuteOfDay % MinutesPerHour);
    return Date;
}
