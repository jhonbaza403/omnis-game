#include "Narrative/Dialogue/OmnisDialogueSubsystem.h"

void UOmnisDialogueSubsystem::StartDialogue(FName SpeakerId, const TArray<FText>& Lines)
{
    if (SpeakerId.IsNone() || Lines.Num() == 0)
    {
        EndDialogue();
        return;
    }

    CurrentSpeaker = SpeakerId;
    CurrentLines = Lines;
    CurrentIndex = 0;
    bActive = true;
    OnDialogueChanged.Broadcast(CurrentSpeaker, CurrentLines[0], false);
}

void UOmnisDialogueSubsystem::AdvanceDialogue()
{
    if (!bActive)
    {
        return;
    }

    ++CurrentIndex;
    if (!CurrentLines.IsValidIndex(CurrentIndex))
    {
        EndDialogue();
        return;
    }

    OnDialogueChanged.Broadcast(CurrentSpeaker, CurrentLines[CurrentIndex], false);
}

void UOmnisDialogueSubsystem::EndDialogue()
{
    const bool WasActive = bActive;
    bActive = false;
    CurrentIndex = INDEX_NONE;
    CurrentLines.Reset();

    if (WasActive)
    {
        OnDialogueChanged.Broadcast(CurrentSpeaker, FText::GetEmpty(), true);
    }
}
