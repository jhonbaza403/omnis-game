#include "AI/Behavior/OmnisBehaviorSubsystem.h"
#include "GameFramework/Actor.h"

void UOmnisBehaviorSubsystem::SetBehaviorForActor(AActor* Actor, FName Behavior)
{
    if (Actor && !Behavior.IsNone())
    {
        Behaviors.Add(Actor->GetFName(), Behavior);
    }
}

FName UOmnisBehaviorSubsystem::GetBehaviorForActor(AActor* Actor) const
{
    if (!Actor)
    {
        return FName(TEXT("Idle"));
    }

    const FName* Behavior = Behaviors.Find(Actor->GetFName());
    return Behavior ? *Behavior : FName(TEXT("Idle"));
}
