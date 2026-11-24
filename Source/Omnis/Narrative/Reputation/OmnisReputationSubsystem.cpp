#include "Narrative/Reputation/OmnisReputationSubsystem.h"

void UOmnisReputationSubsystem::AddReputation(FName SubjectId, int32 Amount)
{
    if (SubjectId.IsNone() || Amount == 0)
    {
        return;
    }

    Reputation.FindOrAdd(SubjectId) = FMath::Clamp(
        Reputation.FindRef(SubjectId) + Amount,
        -100,
        100);
}

int32 UOmnisReputationSubsystem::GetReputation(FName SubjectId) const
{
    return FMath::Clamp(Reputation.FindRef(SubjectId), -100, 100);
}

void UOmnisReputationSubsystem::ImportReputation(const TMap<FName, int32>& Values)
{
    Reputation.Reset();
    for (const TPair<FName, int32>& Pair : Values)
    {
        if (!Pair.Key.IsNone())
        {
            Reputation.Add(Pair.Key, FMath::Clamp(Pair.Value, -100, 100));
        }
    }
}
