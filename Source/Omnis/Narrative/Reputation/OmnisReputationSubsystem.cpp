#include "Narrative/Reputation/OmnisReputationSubsystem.h"

void UOmnisReputationSubsystem::AddReputation(FName SubjectId, int32 Amount)
{
    if (!SubjectId.IsNone() && Amount != 0)
    {
        Reputation.FindOrAdd(SubjectId) += Amount;
    }
}

int32 UOmnisReputationSubsystem::GetReputation(FName SubjectId) const
{
    return Reputation.FindRef(SubjectId);
}
