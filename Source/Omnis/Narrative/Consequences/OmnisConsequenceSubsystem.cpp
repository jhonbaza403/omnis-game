#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"

void UOmnisConsequenceSubsystem::ApplyConsequence(FName ConsequenceId)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    if (!ConsequenceId.IsNone())
    {
        ActiveConsequences.Add(ConsequenceId);
    }
}

void UOmnisConsequenceSubsystem::RemoveConsequence(FName ConsequenceId)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    ActiveConsequences.Remove(ConsequenceId);
}

bool UOmnisConsequenceSubsystem::HasConsequence(FName ConsequenceId) const
{
    return ActiveConsequences.Contains(ConsequenceId);
}

TArray<FName> UOmnisConsequenceSubsystem::ExportConsequences() const
{
    TArray<FName> Result = ActiveConsequences.Array();
    Result.Sort([](const FName& A, const FName& B) { return A.Compare(B) < 0; });
    return Result;
}

void UOmnisConsequenceSubsystem::ImportConsequences(const TArray<FName>& Consequences)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    ActiveConsequences.Reset();
    for (const FName ConsequenceId : Consequences)
    {
        if (!ConsequenceId.IsNone())
        {
            ActiveConsequences.Add(ConsequenceId);
        }
    }
}
