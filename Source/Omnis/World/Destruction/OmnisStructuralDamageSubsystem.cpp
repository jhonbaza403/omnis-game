#include "World/Destruction/OmnisStructuralDamageSubsystem.h"

#include "Engine/World.h"
#include "EngineUtils.h"

bool UOmnisStructuralDamageSubsystem::HasMutationAuthority() const
{
    const UWorld* World = GetWorld();
    return !World || World->GetNetMode() != NM_Client;
}

void UOmnisStructuralDamageSubsystem::RegisterStructure(
    UOmnisStructuralDamageComponent* Component)
{
    if (!HasMutationAuthority() ||
        !IsValid(Component) ||
        Component->PersistentStructureId.IsNone())
    {
        return;
    }

    const FName StructureId = Component->PersistentStructureId;
    if (const TWeakObjectPtr<UOmnisStructuralDamageComponent>* Existing =
        ActiveStructures.Find(StructureId))
    {
        if (Existing->IsValid() && Existing->Get() != Component)
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS duplicate loaded PersistentStructureId '%s'."),
                *StructureId.ToString());
            return;
        }
    }

    ActiveStructures.Add(StructureId, Component);
    if (const FOmnisStructuralDamageSnapshot* Saved = StructureSnapshots.Find(StructureId))
    {
        Component->ImportSnapshot(*Saved);
    }
    else
    {
        RecordSnapshot(Component->ExportSnapshot());
    }
}

void UOmnisStructuralDamageSubsystem::UnregisterStructure(
    UOmnisStructuralDamageComponent* Component)
{
    if (!Component || Component->PersistentStructureId.IsNone())
    {
        return;
    }

    const FName StructureId = Component->PersistentStructureId;
    if (const TWeakObjectPtr<UOmnisStructuralDamageComponent>* Existing =
        ActiveStructures.Find(StructureId))
    {
        if (!Existing->IsValid() || Existing->Get() == Component)
        {
            ActiveStructures.Remove(StructureId);
        }
    }
}

void UOmnisStructuralDamageSubsystem::RecordSnapshot(
    const FOmnisStructuralDamageSnapshot& Snapshot)
{
    if (!HasMutationAuthority() || Snapshot.PersistentStructureId.IsNone())
    {
        return;
    }

    StructureSnapshots.Add(Snapshot.PersistentStructureId, Snapshot);
}

TArray<FOmnisStructuralDamageSnapshot>
UOmnisStructuralDamageSubsystem::ExportSnapshots() const
{
    TArray<FOmnisStructuralDamageSnapshot> Result;
    Result.Reserve(StructureSnapshots.Num());
    for (const TPair<FName, FOmnisStructuralDamageSnapshot>& Pair : StructureSnapshots)
    {
        Result.Add(Pair.Value);
    }

    Result.Sort([](
        const FOmnisStructuralDamageSnapshot& A,
        const FOmnisStructuralDamageSnapshot& B)
    {
        return A.PersistentStructureId.Compare(B.PersistentStructureId) < 0;
    });
    return Result;
}

void UOmnisStructuralDamageSubsystem::ImportSnapshots(
    const TArray<FOmnisStructuralDamageSnapshot>& Snapshots)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    StructureSnapshots.Reset();
    for (const FOmnisStructuralDamageSnapshot& Snapshot : Snapshots)
    {
        if (Snapshot.PersistentStructureId.IsNone())
        {
            continue;
        }
        StructureSnapshots.Add(Snapshot.PersistentStructureId, Snapshot);
    }

    ActiveStructures.Reset();
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor)
        {
            continue;
        }

        TArray<UOmnisStructuralDamageComponent*> Components;
        Actor->GetComponents<UOmnisStructuralDamageComponent>(Components);
        for (UOmnisStructuralDamageComponent* Component : Components)
        {
            RegisterStructure(Component);
        }
    }
}
