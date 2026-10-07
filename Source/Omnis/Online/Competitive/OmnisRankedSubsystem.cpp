#include "Online/Competitive/OmnisRankedSubsystem.h"

void UOmnisRankedSubsystem::RecordMatchResult(bool bWon)
{
    if (bWon)
    {
        ++State.Wins;
        ApplySkillDelta(25);
    }
    else
    {
        ++State.Losses;
        ApplySkillDelta(-15);
    }
}

void UOmnisRankedSubsystem::ApplySkillDelta(int32 Delta)
{
    State.Rating = FMath::Clamp(State.Rating + Delta, 0, 5000);
    RecalculateDivision();
}

void UOmnisRankedSubsystem::RecalculateDivision()
{
    State.Division = FMath::Clamp((State.Rating / 500) + 1, 1, 11);
}

FText UOmnisRankedSubsystem::GetDivisionName() const
{
    static const TCHAR* Names[] =
    {
        TEXT("Bronce"), TEXT("Plata"), TEXT("Oro"), TEXT("Platino"),
        TEXT("Esmeralda"), TEXT("Zafiro"), TEXT("Rango"), TEXT("Élite"),
        TEXT("Maestro"), TEXT("Ascendente"), TEXT("Nexo")
    };

    const int32 Index = FMath::Clamp(
        State.Division - 1,
        0,
        static_cast<int32>(UE_ARRAY_COUNT(Names)) - 1);

    return FText::FromString(Names[Index]);
}
