#include "AI/NPC/OmnisCharacterLifeSubsystem.h"

#include "AI/NPC/OmnisNPCCharacter.h"
#include "Core/Time/OmnisTimeSubsystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"

bool UOmnisCharacterLifeSubsystem::RegisterLocation(
    FName LocationId,
    const FVector& WorldLocation)
{
    if (LocationId.IsNone() || WorldLocation.ContainsNaN())
    {
        return false;
    }

    if (const FVector* Existing = NamedLocations.Find(LocationId))
    {
        if (FVector::DistSquared(*Existing, WorldLocation) > 1.0f)
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS location ID '%s' was registered with conflicting coordinates."),
                *LocationId.ToString());
            return false;
        }
        return true;
    }

    NamedLocations.Add(LocationId, WorldLocation);
    return true;
}

FVector UOmnisCharacterLifeSubsystem::ResolveLocation(
    FName LocationId,
    const FVector& Fallback) const
{
    if (const FVector* Found = NamedLocations.Find(LocationId))
    {
        return *Found;
    }
    return Fallback;
}


bool UOmnisCharacterLifeSubsystem::IsTickable() const
{
    const UWorld* World = GetWorld();
    return World && World->GetNetMode() != NM_Client;
}

void UOmnisCharacterLifeSubsystem::RegisterNPC(AOmnisNPCCharacter* NPC)
{
    if (!NPC || !NPC->HasAuthority())
    {
        return;
    }

    const bool bAlreadyRegistered = RegisteredNPCs.ContainsByPredicate(
        [NPC](const FOmnisRegisteredNPCEntry& Entry)
        {
            return Entry.NPC.Get() == NPC;
        });
    if (bAlreadyRegistered)
    {
        return;
    }

    double CurrentGameSeconds = 0.0;
    if (UWorld* World = GetWorld())
    {
        if (const UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>())
        {
            CurrentGameSeconds = Time->GetElapsedSeconds();
        }
    }

    FOmnisRegisteredNPCEntry Entry;
    Entry.NPC = NPC;
    Entry.LastSimulatedGameSeconds = CurrentGameSeconds;
    RegisteredNPCs.Add(Entry);
}

void UOmnisCharacterLifeSubsystem::UnregisterNPC(AOmnisNPCCharacter* NPC)
{
    if (!NPC)
    {
        return;
    }

    for (int32 Index = RegisteredNPCs.Num() - 1; Index >= 0; --Index)
    {
        if (RegisteredNPCs[Index].NPC.Get() == NPC ||
            !RegisteredNPCs[Index].NPC.IsValid())
        {
            RegisteredNPCs.RemoveAtSwap(Index, 1, EAllowShrinking::No);
        }
    }

    if (RegisteredNPCs.IsEmpty())
    {
        NextNPCIndex = 0;
    }
    else
    {
        NextNPCIndex = NextNPCIndex % RegisteredNPCs.Num();
    }
}

void UOmnisCharacterLifeSubsystem::Tick(float DeltaSeconds)
{
    (void)DeltaSeconds;
    UWorld* World = GetWorld();
    if (!World || World->GetNetMode() == NM_Client || RegisteredNPCs.IsEmpty())
    {
        return;
    }

    const UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>();
    if (!Time)
    {
        return;
    }

    const double CurrentGameSeconds = Time->GetElapsedSeconds();
    TArray<FVector> PlayerLocations;
    for (TActorIterator<AOmnisPlayerCharacter> It(World); It; ++It)
    {
        const AOmnisPlayerCharacter* Player = *It;
        if (Player && (!Player->CharacterState || Player->CharacterState->IsAlive()))
        {
            PlayerLocations.Add(Player->GetActorLocation());
        }
    }

    const float NavigationRadiusSquared =
        FMath::Square(NavigationSimulationRadius);
    const int32 VisitsThisTick = FMath::Min(MaxNPCUpdatesPerTick, RegisteredNPCs.Num());

    for (int32 Visit = 0; Visit < VisitsThisTick && !RegisteredNPCs.IsEmpty(); ++Visit)
    {
        NextNPCIndex = NextNPCIndex % RegisteredNPCs.Num();

        FOmnisRegisteredNPCEntry& Entry = RegisteredNPCs[NextNPCIndex];
        AOmnisNPCCharacter* NPC = Entry.NPC.Get();
        if (!IsValid(NPC))
        {
            RegisteredNPCs.RemoveAtSwap(NextNPCIndex, 1, EAllowShrinking::No);
            continue;
        }

        NextNPCIndex = (NextNPCIndex + 1) % RegisteredNPCs.Num();

        // Loading an older save can move the simulation clock backwards.
        // Rebase this entry rather than waiting for the old timestamp to catch up.
        if (CurrentGameSeconds < Entry.LastSimulatedGameSeconds)
        {
            Entry.LastSimulatedGameSeconds = CurrentGameSeconds;
            continue;
        }

        const double ElapsedGameSeconds =
            CurrentGameSeconds - Entry.LastSimulatedGameSeconds;
        const float UpdateInterval = FMath::Max(1.0f, NPC->GetLifeUpdateInterval());
        if (ElapsedGameSeconds < UpdateInterval)
        {
            continue;
        }

        // Process elapsed simulation in bounded slices. Never drop backlog for
        // NPCs that were off-screen or deferred by the per-frame population budget.
        const double SimulatedStepSeconds = FMath::Min(ElapsedGameSeconds, 10.0);
        Entry.LastSimulatedGameSeconds += SimulatedStepSeconds;

        bool bAllowNavigation = false;
        for (const FVector& PlayerLocation : PlayerLocations)
        {
            if (FVector::DistSquared(NPC->GetActorLocation(), PlayerLocation) <=
                NavigationRadiusSquared)
            {
                bAllowNavigation = true;
                break;
            }
        }

        NPC->SimulateLifeFromPopulationManager(
            static_cast<float>(SimulatedStepSeconds),
            bAllowNavigation);
    }
}

void UOmnisCharacterLifeSubsystem::RebaseRegisteredNPCsToCurrentTime()
{
    UWorld* World = GetWorld();
    const UOmnisTimeSubsystem* Time = World
        ? World->GetSubsystem<UOmnisTimeSubsystem>()
        : nullptr;
    if (!World || !Time || World->GetNetMode() == NM_Client)
    {
        return;
    }

    const double RestoredGameSeconds = Time->GetElapsedSeconds();
    for (FOmnisRegisteredNPCEntry& Entry : RegisteredNPCs)
    {
        if (IsValid(Entry.NPC.Get()))
        {
            Entry.LastSimulatedGameSeconds = RestoredGameSeconds;
        }
    }

    NextNPCIndex = RegisteredNPCs.IsEmpty()
        ? 0
        : NextNPCIndex % RegisteredNPCs.Num();
}
