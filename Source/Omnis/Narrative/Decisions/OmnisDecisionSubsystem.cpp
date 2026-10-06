#include "Narrative/Decisions/OmnisDecisionSubsystem.h"

void UOmnisDecisionSubsystem::RecordDecision(FName DecisionId, int32 Option)
{
    if (DecisionId.IsNone())
    {
        return;
    }

    Decisions.Add(DecisionId, Option);
    OnDecisionRecorded.Broadcast(DecisionId, Option);
}

int32 UOmnisDecisionSubsystem::GetDecision(FName DecisionId) const
{
    const int32* Value = Decisions.Find(DecisionId);
    return Value ? *Value : INDEX_NONE;
}
