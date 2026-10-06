#include "Online/Leaderboards/OmnisLeaderboardSubsystem.h"

void UOmnisLeaderboardSubsystem::SubmitScore(FName BoardId, int32 Score)
{
    if (BoardId.IsNone())
    {
        return;
    }

    int32& Current = Scores.FindOrAdd(BoardId);
    Current = FMath::Max(Current, Score);
}

int32 UOmnisLeaderboardSubsystem::GetScore(FName BoardId) const
{
    return Scores.FindRef(BoardId);
}
