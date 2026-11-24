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

void UOmnisCycleSubsystem::SetPhase(EOmnisCyclePhase NewPhase)
{
    if (State.Phase == NewPhase)
    {
        return;
    }

    State.Phase = NewPhase;
    OnPhaseChanged.Broadcast(NewPhase);
}

void UOmnisCycleSubsystem::RestoreCycleState(const FOmnisCycleState& SavedState)
{
    State.CycleIndex = FMath::Max(0, SavedState.CycleIndex);
    State.WorldAge = FMath::Max(0.0f, FMath::IsFinite(SavedState.WorldAge) ? SavedState.WorldAge : 0.0f);

    const uint8 PhaseValue = static_cast<uint8>(SavedState.Phase);
    if (PhaseValue <= static_cast<uint8>(EOmnisCyclePhase::Rebirth))
    {
        SetPhase(SavedState.Phase);
    }
    else
    {
        SetPhase(EOmnisCyclePhase::Awakening);
    }
}
