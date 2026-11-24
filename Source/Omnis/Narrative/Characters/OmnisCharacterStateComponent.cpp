#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UOmnisCharacterStateComponent::UOmnisCharacterStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UOmnisCharacterStateComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisCharacterStateComponent, bAlive);
    DOREPLIFETIME(UOmnisCharacterStateComponent, ReplicatedRelationships);
    DOREPLIFETIME(UOmnisCharacterStateComponent, Memories);
    DOREPLIFETIME(UOmnisCharacterStateComponent, CurrentGoal);
    DOREPLIFETIME(UOmnisCharacterStateComponent, Hunger);
    DOREPLIFETIME(UOmnisCharacterStateComponent, Energy);
    DOREPLIFETIME(UOmnisCharacterStateComponent, SocialNeed);
}

bool UOmnisCharacterStateComponent::HasMutationAuthority() const
{
    const AActor* Owner = GetOwner();
    return !Owner || Owner->HasAuthority();
}

void UOmnisCharacterStateComponent::SetAlive(bool bNewAlive)
{
    if (!HasMutationAuthority())
    {
        return;
    }
    bAlive = bNewAlive;
    if (!bAlive)
    {
        CurrentGoal = EOmnisCharacterLifeGoal::Idle;
    }
}

void UOmnisCharacterStateComponent::SetRelationship(FName SubjectId, int32 Value)
{
    if (!HasMutationAuthority())
    {
        return;
    }
    if (!SubjectId.IsNone())
    {
        Relationships.Add(SubjectId, FMath::Clamp(Value, -100, 100));
        SyncRelationshipsForReplication();
    }
}

void UOmnisCharacterStateComponent::AdjustRelationship(FName SubjectId, int32 Delta)
{
    if (!HasMutationAuthority())
    {
        return;
    }
    if (!SubjectId.IsNone() && Delta != 0)
    {
        Relationships.FindOrAdd(SubjectId) = FMath::Clamp(
            Relationships.FindRef(SubjectId) + Delta,
            -100,
            100);
        SyncRelationshipsForReplication();
    }
}

void UOmnisCharacterStateComponent::RecordMemory(
    FName EventId,
    FName RelatedCharacterId,
    int32 Importance)
{
    if (!HasMutationAuthority() || EventId.IsNone())
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
    if (!HasMutationAuthority())
    {
        return;
    }
    CurrentGoal = bAlive ? NewGoal : EOmnisCharacterLifeGoal::Idle;
}

void UOmnisCharacterStateComponent::SimulateNeeds(
    float DeltaSeconds,
    EOmnisCharacterLifeGoal Goal,
    bool bReachedGoal)
{
    if (!HasMutationAuthority() || !bAlive || !FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f)
    {
        return;
    }

    // Clamp elapsed time to avoid large offline/suspend spikes changing needs at once.
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

FOmnisCharacterLifeSnapshot UOmnisCharacterStateComponent::ExportSnapshot() const
{
    FOmnisCharacterLifeSnapshot Snapshot;
    Snapshot.bAlive = bAlive;
    Snapshot.Relationships = Relationships;
    Snapshot.Memories = Memories;
    Snapshot.CurrentGoal = CurrentGoal;
    Snapshot.Hunger = Hunger;
    Snapshot.Energy = Energy;
    Snapshot.SocialNeed = SocialNeed;
    return Snapshot;
}

void UOmnisCharacterStateComponent::ImportSnapshot(
    const FOmnisCharacterLifeSnapshot& Snapshot)
{
    if (!HasMutationAuthority())
    {
        return;
    }
    bAlive = Snapshot.bAlive;
    Relationships.Reset();
    for (const TPair<FName, int32>& Pair : Snapshot.Relationships)
    {
        if (!Pair.Key.IsNone())
        {
            Relationships.Add(Pair.Key, FMath::Clamp(Pair.Value, -100, 100));
        }
    }
    SyncRelationshipsForReplication();
    Memories = Snapshot.Memories;
    if (Memories.Num() > 32)
    {
        Memories.RemoveAt(0, Memories.Num() - 32, EAllowShrinking::No);
    }
    for (FOmnisCharacterMemoryEntry& Memory : Memories)
    {
        Memory.Importance = FMath::Clamp(Memory.Importance, 0, 100);
    }

    const uint8 GoalValue = static_cast<uint8>(Snapshot.CurrentGoal);
    const bool bValidGoal = GoalValue <= static_cast<uint8>(EOmnisCharacterLifeGoal::SeekSafety);
    CurrentGoal = bAlive && bValidGoal
        ? Snapshot.CurrentGoal
        : EOmnisCharacterLifeGoal::Idle;
    Hunger = FMath::Clamp(FMath::IsFinite(Snapshot.Hunger) ? Snapshot.Hunger : 0.0f, 0.0f, 100.0f);
    Energy = FMath::Clamp(FMath::IsFinite(Snapshot.Energy) ? Snapshot.Energy : 100.0f, 0.0f, 100.0f);
    SocialNeed = FMath::Clamp(FMath::IsFinite(Snapshot.SocialNeed) ? Snapshot.SocialNeed : 0.0f, 0.0f, 100.0f);
}

void UOmnisCharacterStateComponent::OnRep_Alive()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    if (!bAlive)
    {
        Character->Tags.AddUnique(TEXT("Omnis.Dead"));
        if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
        {
            Movement->DisableMovement();
        }
    }
    else
    {
        Character->Tags.Remove(TEXT("Omnis.Dead"));
        if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
        {
            Movement->SetMovementMode(MOVE_Walking);
        }
    }
}

void UOmnisCharacterStateComponent::SyncRelationshipsForReplication()
{
    ReplicatedRelationships.Reset();
    ReplicatedRelationships.Reserve(Relationships.Num());

    for (const TPair<FName, int32>& Pair : Relationships)
    {
        FOmnisReplicatedRelationship Entry;
        Entry.SubjectId = Pair.Key;
        Entry.Value = Pair.Value;
        ReplicatedRelationships.Add(Entry);
    }

    ReplicatedRelationships.Sort(
        [](const FOmnisReplicatedRelationship& A, const FOmnisReplicatedRelationship& B)
        {
            return A.SubjectId.Compare(B.SubjectId) < 0;
        });
}

void UOmnisCharacterStateComponent::OnRep_Relationships()
{
    Relationships.Reset();
    for (const FOmnisReplicatedRelationship& Entry : ReplicatedRelationships)
    {
        if (!Entry.SubjectId.IsNone())
        {
            Relationships.Add(Entry.SubjectId, FMath::Clamp(Entry.Value, -100, 100));
        }
    }
}

bool UOmnisCharacterStateComponent::HasMemory(FName EventId) const
{
    if (EventId.IsNone())
    {
        return false;
    }

    return Memories.ContainsByPredicate(
        [EventId](const FOmnisCharacterMemoryEntry& Entry)
        {
            return Entry.EventId == EventId;
        });
}
