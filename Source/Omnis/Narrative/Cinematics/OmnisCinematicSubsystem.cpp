#include "Narrative/Cinematics/OmnisCinematicSubsystem.h"

void UOmnisCinematicSubsystem::PlaySequence(FName SequenceId)
{
    if (!SequenceId.IsNone())
    {
        ActiveSequence = SequenceId;
    }
}

void UOmnisCinematicSubsystem::StopSequence()
{
    ActiveSequence = NAME_None;
}
