#include "World/Destruction/OmnisStructuralDamageComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "World/Destruction/OmnisStructuralDamageSubsystem.h"

UOmnisStructuralDamageComponent::UOmnisStructuralDamageComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UOmnisStructuralDamageComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!HasMutationAuthority())
    {
        return;
    }

    if (PersistentStructureId.IsNone())
    {
        UE_LOG(LogTemp, Error,
            TEXT("OMNIS structural damage component on '%s' has no PersistentStructureId."),
            GetOwner() ? *GetOwner()->GetName() : TEXT("<no owner>"));
        return;
    }

    if (PartStates.IsEmpty())
    {
        InitializePartStates();
    }

    ApplyAllVisualStates();

    if (UWorld* World = GetWorld())
    {
        if (UOmnisStructuralDamageSubsystem* Damage =
            World->GetSubsystem<UOmnisStructuralDamageSubsystem>())
        {
            Damage->RegisterStructure(this);
        }
    }
}

void UOmnisStructuralDamageComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UOmnisStructuralDamageSubsystem* Damage =
            World->GetSubsystem<UOmnisStructuralDamageSubsystem>())
        {
            Damage->UnregisterStructure(this);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void UOmnisStructuralDamageComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisStructuralDamageComponent, PartStates);
}

bool UOmnisStructuralDamageComponent::HasMutationAuthority() const
{
    const AActor* Owner = GetOwner();
    return !Owner || Owner->HasAuthority();
}

void UOmnisStructuralDamageComponent::InitializePartStates()
{
    PartStates.Reset();
    for (const FOmnisStructuralPartDefinition& Definition : PartDefinitions)
    {
        if (Definition.PartId.IsNone() ||
            Definition.ComponentTag.IsNone() ||
            !FMath::IsFinite(Definition.MaxDurability) ||
            Definition.MaxDurability <= 0.0f)
        {
            continue;
        }

        if (PartStates.ContainsByPredicate(
            [&Definition](const FOmnisStructuralPartState& Existing)
            {
                return Existing.PartId == Definition.PartId;
            }))
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS structure '%s' has duplicate part ID '%s'."),
                *PersistentStructureId.ToString(),
                *Definition.PartId.ToString());
            continue;
        }

        FOmnisStructuralPartState State;
        State.PartId = Definition.PartId;
        State.ComponentTag = Definition.ComponentTag;
        State.MaxDurability = Definition.MaxDurability;
        State.CurrentDurability = Definition.MaxDurability;
        State.bDestroyed = false;
        PartStates.Add(State);
    }
}

bool UOmnisStructuralDamageComponent::ApplyStructuralDamage(
    FName PartId,
    float Damage)
{
    if (!HasMutationAuthority() ||
        PartId.IsNone() ||
        !FMath::IsFinite(Damage) ||
        Damage <= 0.0f)
    {
        return false;
    }

    FOmnisStructuralPartState* Part = PartStates.FindByPredicate(
        [PartId](const FOmnisStructuralPartState& Candidate)
        {
            return Candidate.PartId == PartId;
        });
    if (!Part || Part->bDestroyed)
    {
        return false;
    }

    Part->CurrentDurability = FMath::Max(0.0f, Part->CurrentDurability - Damage);
    Part->bDestroyed = Part->CurrentDurability <= 0.0f;
    ApplyVisualState(*Part);
    NotifyStateChanged();
    return true;
}

bool UOmnisStructuralDamageComponent::RepairStructuralPart(
    FName PartId,
    float RepairAmount)
{
    if (!HasMutationAuthority() ||
        PartId.IsNone() ||
        !FMath::IsFinite(RepairAmount) ||
        RepairAmount <= 0.0f)
    {
        return false;
    }

    FOmnisStructuralPartState* Part = PartStates.FindByPredicate(
        [PartId](const FOmnisStructuralPartState& Candidate)
        {
            return Candidate.PartId == PartId;
        });
    if (!Part || Part->CurrentDurability >= Part->MaxDurability)
    {
        return false;
    }

    Part->CurrentDurability = FMath::Min(
        Part->MaxDurability,
        Part->CurrentDurability + RepairAmount);
    Part->bDestroyed = Part->CurrentDurability <= 0.0f;
    ApplyVisualState(*Part);
    NotifyStateChanged();
    return true;
}

float UOmnisStructuralDamageComponent::GetPartDurability(FName PartId) const
{
    const FOmnisStructuralPartState* Part = PartStates.FindByPredicate(
        [PartId](const FOmnisStructuralPartState& Candidate)
        {
            return Candidate.PartId == PartId;
        });
    return Part ? Part->CurrentDurability : 0.0f;
}

bool UOmnisStructuralDamageComponent::IsPartDestroyed(FName PartId) const
{
    const FOmnisStructuralPartState* Part = PartStates.FindByPredicate(
        [PartId](const FOmnisStructuralPartState& Candidate)
        {
            return Candidate.PartId == PartId;
        });
    return Part && Part->bDestroyed;
}

FOmnisStructuralDamageSnapshot UOmnisStructuralDamageComponent::ExportSnapshot() const
{
    FOmnisStructuralDamageSnapshot Snapshot;
    Snapshot.PersistentStructureId = PersistentStructureId;
    Snapshot.Parts = PartStates;
    Snapshot.Parts.Sort([](
        const FOmnisStructuralPartState& A,
        const FOmnisStructuralPartState& B)
    {
        return A.PartId.Compare(B.PartId) < 0;
    });
    return Snapshot;
}

void UOmnisStructuralDamageComponent::ImportSnapshot(
    const FOmnisStructuralDamageSnapshot& Snapshot)
{
    if (!HasMutationAuthority() ||
        Snapshot.PersistentStructureId.IsNone() ||
        Snapshot.PersistentStructureId != PersistentStructureId)
    {
        return;
    }

    for (FOmnisStructuralPartState& LocalPart : PartStates)
    {
        const FOmnisStructuralPartState* SavedPart = Snapshot.Parts.FindByPredicate(
            [&LocalPart](const FOmnisStructuralPartState& Candidate)
            {
                return Candidate.PartId == LocalPart.PartId;
            });
        if (!SavedPart)
        {
            continue;
        }

        LocalPart.MaxDurability = FMath::Max(1.0f, LocalPart.MaxDurability);
        LocalPart.CurrentDurability = FMath::Clamp(
            SavedPart->CurrentDurability,
            0.0f,
            LocalPart.MaxDurability);
        LocalPart.bDestroyed = LocalPart.CurrentDurability <= 0.0f;
    }

    ApplyAllVisualStates();
    if (AActor* Owner = GetOwner())
    {
        Owner->ForceNetUpdate();
    }
    OnDamageChanged.Broadcast(PersistentStructureId);
}

void UOmnisStructuralDamageComponent::ApplyVisualState(
    const FOmnisStructuralPartState& PartState)
{
    AActor* Owner = GetOwner();
    if (!Owner || PartState.ComponentTag.IsNone())
    {
        return;
    }

    TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
    Owner->GetComponents(PrimitiveComponents);
    for (UPrimitiveComponent* Primitive : PrimitiveComponents)
    {
        if (!Primitive || !Primitive->ComponentHasTag(PartState.ComponentTag))
        {
            continue;
        }

        Primitive->SetVisibility(!PartState.bDestroyed, true);
        Primitive->SetCollisionEnabled(
            PartState.bDestroyed
                ? ECollisionEnabled::NoCollision
                : ECollisionEnabled::QueryAndPhysics);
    }
}

void UOmnisStructuralDamageComponent::ApplyAllVisualStates()
{
    for (const FOmnisStructuralPartState& PartState : PartStates)
    {
        ApplyVisualState(PartState);
    }
}

void UOmnisStructuralDamageComponent::NotifyStateChanged()
{
    ApplyAllVisualStates();
    if (AActor* Owner = GetOwner())
    {
        Owner->ForceNetUpdate();
    }
    if (UWorld* World = GetWorld())
    {
        if (UOmnisStructuralDamageSubsystem* Damage =
            World->GetSubsystem<UOmnisStructuralDamageSubsystem>())
        {
            Damage->RecordSnapshot(ExportSnapshot());
        }
    }
    OnDamageChanged.Broadcast(PersistentStructureId);
}

void UOmnisStructuralDamageComponent::OnRep_PartStates()
{
    ApplyAllVisualStates();
    OnDamageChanged.Broadcast(PersistentStructureId);
}
