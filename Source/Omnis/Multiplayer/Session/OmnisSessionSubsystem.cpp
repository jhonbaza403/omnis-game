#include "Multiplayer/Session/OmnisSessionSubsystem.h"

bool UOmnisSessionSubsystem::CreateSession(FName NewSessionId, int32 InMaxPlayers)
{
    if (bSessionActive || NewSessionId.IsNone() || InMaxPlayers <= 0)
    {
        return false;
    }

    SessionId = NewSessionId;
    MaxPlayers = InMaxPlayers;
    bSessionActive = true;
    return true;
}

void UOmnisSessionSubsystem::LeaveSession()
{
    bSessionActive = false;
    SessionId = NAME_None;
    MaxPlayers = 0;
}
