#include "Narrative/Characters/OmnisCharacterStateComponent.h"

UOmnisCharacterStateComponent::UOmnisCharacterStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UOmnisCharacterStateComponent::SetAlive(bool bNewAlive)
{
    bAlive = bNewAlive;
    if (!bAlive)
    {
        CurrentGoal = EOmnisCharacterLifeGoal::Idle;
    }
}

void UOmnisCharacterStateComponent::SetRelationship(FName SubjectId, int32 Value)
{
    if (!SubjectId.IsNone())
    {
        Relationships.Add(SubjectId, FMath::Clamp(Value, -100, 100));
    }
}

void UOmnisCharacterStateComponent::AdjustRelationship(FName SubjectId, int32 Delta)
{
    if (!SubjectId.IsNone() && Delta != 0)
    {
        Relationships.FindOrAdd(SubjectId) = FMath::Clamp(
            Relationships.FindRef(SubjectId) + Delta,
            -100,
            100);
    }
}

void UOmnisCharacterStateComponent::RecordMemory(
    FName EventId,
    FName RelatedCharacterId,
    int32 Importance)
{
    if (EventId.IsNone())
    {
        return;
    }

    FOmnisCharacterMemoryEntry Entry;
    Entry.EventId = EventId;
    Entry.RelatedCharacterId = RelatedCharacterId;
    Entry.Importance = FMath::Clamp(Importance, 0, 100);
    Memories.Add(Entry);

    constexpr int32 MaxMemories = 32;
    if (Memories.Num() > MaxMemories)
    {
        Memories.RemoveAt(0, Memories.Num() - MaxMemories, EAllowShrinking::No);
    }
}

void UOmnisCharacterStateComponent::SetCurrentGoal(EOmnisCharacterLifeGoal NewGoal)
{
    CurrentGoal = bAlive ? NewGoal : EOmnisCharacterLifeGoal::Idle;
}

void UOmnisCharacterStateComponent::SimulateNeeds(
    float DeltaSeconds,
    EOmnisCharacterLifeGoal Goal,
    bool bReachedGoal)
{
    if (!bAlive || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f)
    {
        return;
    }

    const float Step = FMath::Min(DeltaSeconds, 10.0f);
    Hunger = FMath::Clamp(Hunger + Step * 0.035f, 0.0f, 100.0f);
    SocialNeed = FMath::Clamp(SocialNeed + Step * 0.018f, 0.0f, 100.0f);

    if (Goal == EOmnisCharacterLifeGoal::Rest && bReachedGoal)
    {
        Energy = FMath::Clamp(Energy + Step * 0.10f, 0.0f, 100.0f);
    }
    else
    {
        Energy = FMath::Clamp(Energy - Step * 0.012f, 0.0f, 100.0f);
    }

    if (Goal == EOmnisCharacterLifeGoal::Eat && bReachedGoal)
    {
        Hunger = FMath::Clamp(Hunger - Step * 0.18f, 0.0f, 100.0f);
    }

    if (Goal == EOmnisCharacterLifeGoal::Socialize && bReachedGoal)
    {
        SocialNeed = FMath::Clamp(SocialNeed - Step * 0.08f, 0.0f, 100.0f);
    }
}
