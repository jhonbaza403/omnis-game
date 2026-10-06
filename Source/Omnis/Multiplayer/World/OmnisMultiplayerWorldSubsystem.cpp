#include "Multiplayer/World/OmnisMultiplayerWorldSubsystem.h"

void UOmnisMultiplayerWorldSubsystem::SetWorldId(FName NewWorldId)
{
    if (!NewWorldId.IsNone())
    {
        WorldId = NewWorldId;
    }
}
