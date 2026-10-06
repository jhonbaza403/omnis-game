#include "Online/Matchmaking/OmnisMatchmakingSubsystem.h"

void UOmnisMatchmakingSubsystem::Enqueue(FName NewPlaylistId)
{
    if (!NewPlaylistId.IsNone())
    {
        PlaylistId = NewPlaylistId;
        bQueued = true;
    }
}

void UOmnisMatchmakingSubsystem::Dequeue()
{
    PlaylistId = NAME_None;
    bQueued = false;
}
