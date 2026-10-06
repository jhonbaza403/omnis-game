#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"

void UOmnisConsequenceSubsystem::ApplyConsequence(FName ConsequenceId)
{
    if (!ConsequenceId.IsNone())
    {
        ActiveConsequences.Add(ConsequenceId);
    }
}

void UOmnisConsequenceSubsystem::RemoveConsequence(FName ConsequenceId)
{
    ActiveConsequences.Remove(ConsequenceId);
}

bool UOmnisConsequenceSubsystem::HasConsequence(FName ConsequenceId) const
{
    return ActiveConsequences.Contains(ConsequenceId);
}
