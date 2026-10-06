#include "AI/Behavior/OmnisBehaviorSubsystem.h"

void UOmnisBehaviorSubsystem::SetBehaviorForActor(AActor* Actor, FName Behavior)
{
    if (Actor && !Behavior.IsNone())
    {
        Behaviors.Add(Actor, Behavior);
    }
}

FName UOmnisBehaviorSubsystem::GetBehaviorForActor(AActor* Actor) const
{
    const FName* Behavior = Behaviors.Find(Actor);
    return Behavior ? *Behavior : FName(TEXT("Idle"));
}
