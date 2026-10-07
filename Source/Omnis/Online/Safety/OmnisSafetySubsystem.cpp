#include "Online/Safety/OmnisSafetySubsystem.h"

bool UOmnisSafetySubsystem::BlockPlayer(FName UserId)
{
    if (UserId.IsNone())
    {
        return false;
    }

    return BlockedPlayers.Add(UserId) > 0;
}

bool UOmnisSafetySubsystem::UnblockPlayer(FName UserId)
{
    return BlockedPlayers.Remove(UserId) > 0;
}

bool UOmnisSafetySubsystem::ReportPlayer(
    FName ReporterId,
    FName TargetId,
    EOmnisSafetyReason Reason,
    FString Context)
{
    if (ReporterId.IsNone() || TargetId.IsNone() || ReporterId == TargetId)
    {
        return false;
    }

    Context.TrimStartAndEndInline();
    Context = Context.Left(500);

    FOmnisSafetyReport Report;
    Report.ReporterId = ReporterId;
    Report.TargetId = TargetId;
    Report.Reason = Reason;
    Report.Context = Context;
    Reports.Insert(Report, 0);

    if (Reports.Num() > 100)
    {
        Reports.SetNum(100);
    }

    return true;
}

bool UOmnisSafetySubsystem::IsBlocked(FName UserId) const
{
    return !UserId.IsNone() && BlockedPlayers.Contains(UserId);
}

bool UOmnisSafetySubsystem::CanInteract(FName UserId) const
{
    return !UserId.IsNone() && !IsBlocked(UserId);
}
