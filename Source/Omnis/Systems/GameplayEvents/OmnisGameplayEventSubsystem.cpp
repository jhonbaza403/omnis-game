#include "Systems/GameplayEvents/OmnisGameplayEventSubsystem.h"

void UOmnisGameplayEventSubsystem::Emit(FName EventId, AActor* Instigator)
{
    if (!EventId.IsNone())
    {
        OnEvent.Broadcast(EventId, Instigator);
    }
}
