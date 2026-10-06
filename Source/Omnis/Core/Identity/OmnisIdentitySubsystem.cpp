#include "Core/Identity/OmnisIdentitySubsystem.h"

void UOmnisIdentitySubsystem::SetPlayerId(FString NewPlayerId)
{
    NewPlayerId.TrimStartAndEndInline();
    PlayerId = NewPlayerId.Left(64);
}
