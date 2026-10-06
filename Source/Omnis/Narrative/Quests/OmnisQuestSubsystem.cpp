#include "Narrative/Quests/OmnisQuestSubsystem.h"

void UOmnisQuestSubsystem::SetQuestState(FName QuestId, EOmnisQuestState State)
{
    if (!QuestId.IsNone())
    {
        QuestStates.Add(QuestId, State);
    }
}

EOmnisQuestState UOmnisQuestSubsystem::GetQuestState(FName QuestId) const
{
    const EOmnisQuestState* State = QuestStates.Find(QuestId);
    return State ? *State : EOmnisQuestState::Inactive;
}
