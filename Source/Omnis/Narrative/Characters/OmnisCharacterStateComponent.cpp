#include "Narrative/Characters/OmnisCharacterStateComponent.h"

UOmnisCharacterStateComponent::UOmnisCharacterStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UOmnisCharacterStateComponent::SetAlive(bool bNewAlive)
{
    bAlive = bNewAlive;
}

void UOmnisCharacterStateComponent::SetRelationship(FName SubjectId, int32 Value)
{
    if (!SubjectId.IsNone())
    {
        Relationships.Add(SubjectId, FMath::Clamp(Value, -100, 100));
    }
}

int32 UOmnisCharacterStateComponent::GetRelationship(FName SubjectId) const
{
    return Relationships.FindRef(SubjectId);
}
