#include "Gameplay/Abilities/OmnisAbilityComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameStateBase.h"

UOmnisAbilityComponent::UOmnisAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UOmnisAbilityComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UOmnisAbilityComponent, ReplicatedCooldowns);
}

bool UOmnisAbilityComponent::ActivateAbility(FName AbilityId)
{
    AActor* Owner = GetOwner();
    if (!Owner || AbilityId.IsNone())
    {
        return false;
    }

    if (!Owner->HasAuthority())
    {
        ServerActivateAbility(AbilityId);
        return true;
    }

    return ActivateAbilityOnAuthority(AbilityId);
}

void UOmnisAbilityComponent::ServerActivateAbility_Implementation(FName AbilityId)
{
    ActivateAbilityOnAuthority(AbilityId);
}

bool UOmnisAbilityComponent::ActivateAbilityOnAuthority(FName AbilityId)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority() || AbilityId.IsNone())
    {
        return false;
    }

    PruneExpiredCooldowns();
    if (IsOnCooldown(AbilityId))
    {
        return false;
    }

    SetCooldown(AbilityId, 1.0f);
    MulticastAbilityActivated(AbilityId);
    return true;
}

void UOmnisAbilityComponent::SetCooldown(FName AbilityId, float Duration)
{
    AActor* Owner = GetOwner();
    if ((Owner && !Owner->HasAuthority()) || AbilityId.IsNone() ||
        !FMath::IsFinite(Duration) || Duration <= 0.0f)
    {
        return;
    }

    Cooldowns.Add(AbilityId, GetCurrentServerTime() + Duration);
    SyncReplicatedCooldowns();
}

bool UOmnisAbilityComponent::IsOnCooldown(FName AbilityId) const
{
    const float* Expiry = Cooldowns.Find(AbilityId);
    return Expiry && GetCurrentServerTime() < *Expiry;
}

float UOmnisAbilityComponent::GetRemainingCooldown(FName AbilityId) const
{
    const float* Expiry = Cooldowns.Find(AbilityId);
    return Expiry ? FMath::Max(0.0f, *Expiry - GetCurrentServerTime()) : 0.0f;
}

float UOmnisAbilityComponent::GetCurrentServerTime() const
{
    if (UWorld* World = GetWorld())
    {
        if (const AGameStateBase* GameState = World->GetGameState())
        {
            return GameState->GetServerWorldTimeSeconds();
        }
        return World->GetTimeSeconds();
    }
    return 0.0f;
}

void UOmnisAbilityComponent::PruneExpiredCooldowns()
{
    const float Now = GetCurrentServerTime();
    bool bChanged = false;

    for (auto It = Cooldowns.CreateIterator(); It; ++It)
    {
        if (It.Value() <= Now)
        {
            It.RemoveCurrent();
            bChanged = true;
        }
    }

    if (bChanged)
    {
        SyncReplicatedCooldowns();
    }
}

void UOmnisAbilityComponent::SyncReplicatedCooldowns()
{
    ReplicatedCooldowns.Reset();
    ReplicatedCooldowns.Reserve(Cooldowns.Num());

    for (const TPair<FName, float>& Pair : Cooldowns)
    {
        FOmnisAbilityCooldownEntry Entry;
        Entry.AbilityId = Pair.Key;
        Entry.EndServerTime = Pair.Value;
        ReplicatedCooldowns.Add(Entry);
    }

    ReplicatedCooldowns.Sort(
        [](const FOmnisAbilityCooldownEntry& A, const FOmnisAbilityCooldownEntry& B)
        {
            return A.AbilityId.Compare(B.AbilityId) < 0;
        });

    if (AActor* Owner = GetOwner())
    {
        Owner->ForceNetUpdate();
    }
}

void UOmnisAbilityComponent::OnRep_CooldownEntries()
{
    Cooldowns.Reset();
    for (const FOmnisAbilityCooldownEntry& Entry : ReplicatedCooldowns)
    {
        if (!Entry.AbilityId.IsNone() && FMath::IsFinite(Entry.EndServerTime))
        {
            Cooldowns.Add(Entry.AbilityId, Entry.EndServerTime);
        }
    }
}

void UOmnisAbilityComponent::MulticastAbilityActivated_Implementation(FName AbilityId)
{
    OnAbilityActivated.Broadcast(AbilityId);
}

TArray<FOmnisSavedAbilityCooldown> UOmnisAbilityComponent::ExportCooldowns() const
{
    TArray<FOmnisSavedAbilityCooldown> Result;
    const float Now = GetCurrentServerTime();

    for (const TPair<FName, float>& Pair : Cooldowns)
    {
        const float Remaining = Pair.Value - Now;
        if (Pair.Key.IsNone() || Remaining <= 0.0f)
        {
            continue;
        }

        FOmnisSavedAbilityCooldown Snapshot;
        Snapshot.AbilityId = Pair.Key;
        Snapshot.RemainingSeconds = Remaining;
        Result.Add(Snapshot);
    }

    Result.Sort([](const FOmnisSavedAbilityCooldown& A, const FOmnisSavedAbilityCooldown& B)
    {
        return A.AbilityId.Compare(B.AbilityId) < 0;
    });
    return Result;
}

void UOmnisAbilityComponent::ImportCooldowns(
    const TArray<FOmnisSavedAbilityCooldown>& SavedCooldowns)
{
    AActor* Owner = GetOwner();
    if (Owner && !Owner->HasAuthority())
    {
        return;
    }

    Cooldowns.Reset();
    const float Now = GetCurrentServerTime();
    for (const FOmnisSavedAbilityCooldown& Saved : SavedCooldowns)
    {
        if (Saved.AbilityId.IsNone() || !FMath::IsFinite(Saved.RemainingSeconds) ||
            Saved.RemainingSeconds <= 0.0f)
        {
            continue;
        }

        Cooldowns.Add(Saved.AbilityId, Now + FMath::Min(Saved.RemainingSeconds, 3600.0f));
    }

    SyncReplicatedCooldowns();
}
