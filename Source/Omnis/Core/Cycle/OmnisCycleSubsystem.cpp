#include "Core/Cycle/OmnisCycleSubsystem.h"

void UOmnisCycleSubsystem::BeginCycle()
{
    SetPhase(EOmnisCyclePhase::Awakening);
}

void UOmnisCycleSubsystem::EnterNexus()
{
    SetPhase(EOmnisCyclePhase::Nexus);
}

void UOmnisCycleSubsystem::BeginRebirth()
{
    ++State.CycleIndex;
    SetPhase(EOmnisCyclePhase::Rebirth);
}

void UOmnisCycleSubsystem::AdvanceWorldAge(float DeltaYears)
{
    State.WorldAge = FMath::Max(0.0f, State.WorldAge + DeltaYears);
}

void UOmnisCycleSubsystem::RestoreState(const FOmnisCycleState& SavedState)
{
    State = SavedState;
    OnPhaseChanged.Broadcast(State.Phase);
}

void UOmnisCycleSubsystem::SetPhase(EOmnisCyclePhase NewPhase)
{
    if (State.Phase == NewPhase)
    {
        return;
    }

    State.Phase = NewPhase;
    OnPhaseChanged.Broadcast(NewPhase);
}
