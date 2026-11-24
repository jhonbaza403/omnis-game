#include "Multiplayer/Persistence/OmnisPersistenceSubsystem.h"
#include "Multiplayer/World/OmnisReplicatedWorldStateActor.h"

#include "AI/NPC/OmnisNPCCharacter.h"
#include "AI/NPC/OmnisCharacterLifeSubsystem.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Core/Save/OmnisSaveGame.h"
#include "Core/Time/OmnisTimeSubsystem.h"
#include "EngineUtils.h"
#include "Gameplay/Character/OmnisPlayerCharacter.h"
#include "Gameplay/Abilities/OmnisAbilityComponent.h"
#include "Gameplay/Equipment/OmnisEquipmentComponent.h"
#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"
#include "Narrative/Factions/OmnisFactionSubsystem.h"
#include "Narrative/Quests/OmnisQuestSubsystem.h"
#include "Narrative/Reputation/OmnisReputationSubsystem.h"
#include "Systems/Damage/OmnisHealthComponent.h"
#include "World/WorldState/OmnisWorldStateSubsystem.h"

namespace
{
    constexpr int32 CurrentSaveSchemaVersion = 4;

    bool IsValidSlotName(const FString& SlotName)
    {
        return !SlotName.IsEmpty() &&
            SlotName.Len() <= 64 &&
            !SlotName.Contains(TEXT("/")) &&
            !SlotName.Contains(TEXT("\\")) &&
            !SlotName.Contains(TEXT(".."));
    }

    bool CanSaveForCurrentNetMode(const UWorld* World)
    {
        return World && World->GetNetMode() != NM_Client;
    }
}

void UOmnisPersistenceSubsystem::ImportPersistentInts(const TMap<FName, int32>& Values)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    PersistentInts.Reset();
    for (const TPair<FName, int32>& Pair : Values)
    {
        if (!Pair.Key.IsNone())
        {
            PersistentInts.Add(Pair.Key, Pair.Value);
        }
    }
}

void UOmnisPersistenceSubsystem::SetPersistentInt(FName Key, int32 Value)
{
    if (GetWorld() && GetWorld()->GetNetMode() == NM_Client)
    {
        return;
    }
    if (!Key.IsNone())
    {
        PersistentInts.Add(Key, Value);
    }
}

int32 UOmnisPersistenceSubsystem::GetPersistentInt(FName Key, int32 DefaultValue) const
{
    const int32* Value = PersistentInts.Find(Key);
    return Value ? *Value : DefaultValue;
}

bool UOmnisPersistenceSubsystem::SaveCurrentGame(const FString& SlotName)
{
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = GetGameInstance();
    if (!IsValidSlotName(SlotName) || !World || !GameInstance ||
        !CanSaveForCurrentNetMode(World))
    {
        return false;
    }

    UOmnisSaveGame* Save = Cast<UOmnisSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UOmnisSaveGame::StaticClass()));
    if (!Save)
    {
        return false;
    }

    Save->SchemaVersion = CurrentSaveSchemaVersion;
    Save->SavedAtUtc = FDateTime::UtcNow();
    Save->PersistentInts = PersistentInts;

    if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
    {
        Save->CycleState = Cycle->GetCycleState();
    }
    if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
    {
        Save->NexusProgression = Nexus->ExportProgression();
        Save->PermanentUpgradePoints = Save->NexusProgression.PermanentUpgradePoints;
    }
    if (UOmnisQuestSubsystem* Quests = GameInstance->GetSubsystem<UOmnisQuestSubsystem>())
    {
        Save->QuestProgress = Quests->ExportProgress();
    }
    if (UOmnisConsequenceSubsystem* Consequences =
        GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
    {
        Save->ActiveConsequences = Consequences->ExportConsequences();
    }
    if (UOmnisReputationSubsystem* Reputation =
        GameInstance->GetSubsystem<UOmnisReputationSubsystem>())
    {
        Save->ReputationValues = Reputation->ExportReputation();
    }
    if (UOmnisFactionSubsystem* Factions =
        GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
    {
        Save->FactionStandings = Factions->ExportStandings();
    }
    if (UOmnisWorldStateSubsystem* WorldState =
        World->GetSubsystem<UOmnisWorldStateSubsystem>())
    {
        Save->WorldStateValues = WorldState->ExportStates();
    }
    if (UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>())
    {
        Save->WorldElapsedSeconds = Time->GetElapsedSeconds();
        Save->WorldTimeScale = Time->GetTimeScale();
    }

    TSet<FName> SavedCharacterIds;
    if (AOmnisPlayerCharacter* Player =
        Cast<AOmnisPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0)))
    {
        if (Player->CharacterState && Player->CharacterState->IsAlive())
        {
            FOmnisSavedCharacterState Character;
            Character.PersistentId = TEXT("Player");
            Character.bIsPlayer = true;
            Character.Transform = Player->GetActorTransform();
            Character.LifeState = Player->CharacterState->ExportSnapshot();
            if (Player->Health)
            {
                Character.Health = Player->Health->GetHealth();
                Character.MaxHealth = Player->Health->GetMaxHealth();
            }
            if (Player->Inventory)
            {
                Character.InventoryItems = Player->Inventory->ExportItems();
            }
            if (Player->Equipment)
            {
                Character.EquippedItems = Player->Equipment->ExportEquipment();
            }
            if (Player->Abilities)
            {
                Character.AbilityCooldowns = Player->Abilities->ExportCooldowns();
            }
            Save->Characters.Add(Character);
            SavedCharacterIds.Add(Character.PersistentId);
        }
    }

    for (TActorIterator<AOmnisNPCCharacter> It(World); It; ++It)
    {
        AOmnisNPCCharacter* NPC = *It;
        if (!NPC || !NPC->CharacterState)
        {
            continue;
        }
        const FName PersistentId = NPC->GetPersistentId();
        if (PersistentId.IsNone())
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS save aborted: NPC '%s' has no explicit PersistentId."),
                *NPC->GetName());
            return false;
        }
        if (SavedCharacterIds.Contains(PersistentId))
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS save aborted: duplicate PersistentId '%s'."),
                *PersistentId.ToString());
            return false;
        }

        FOmnisSavedCharacterState Character;
        Character.PersistentId = PersistentId;
        Character.bIsPlayer = false;
        Character.Transform = NPC->GetActorTransform();
        Character.LifeState = NPC->CharacterState->ExportSnapshot();
        if (NPC->Health)
        {
            Character.Health = NPC->Health->GetHealth();
            Character.MaxHealth = NPC->Health->GetMaxHealth();
        }
        Save->Characters.Add(Character);
        SavedCharacterIds.Add(Character.PersistentId);
    }

    return UGameplayStatics::SaveGameToSlot(Save, SlotName, 0);
}

bool UOmnisPersistenceSubsystem::LoadCurrentGame(const FString& SlotName)
{
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = GetGameInstance();
    if (!IsValidSlotName(SlotName) || !World || !GameInstance ||
        !CanSaveForCurrentNetMode(World) ||
        !UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        return false;
    }

    UOmnisSaveGame* Save = Cast<UOmnisSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!Save || (Save->SchemaVersion != 0 &&
        Save->SchemaVersion != 2 &&
        Save->SchemaVersion != 3 &&
        Save->SchemaVersion != CurrentSaveSchemaVersion))
    {
        UE_LOG(LogTemp, Error,
            TEXT("OMNIS save rejected: unsupported schema version."));
        return false;
    }

    // The top-level field predates NexusProgression. Merge it for every schema
    // so saves created before this field existed cannot silently lose points.
    // New saves write both fields with the same value, so this is idempotent.
    Save->NexusProgression.PermanentUpgradePoints = FMath::Max(
        Save->NexusProgression.PermanentUpgradePoints,
        Save->PermanentUpgradePoints);

    // Schema 2 predates player inventory/equipment/cooldown persistence;
    // absent arrays remain valid empty inventories/loadouts.

    // Validate every saved character identity before changing any live subsystem.
    TMap<FName, AOmnisNPCCharacter*> NPCsById;
    for (TActorIterator<AOmnisNPCCharacter> It(World); It; ++It)
    {
        AOmnisNPCCharacter* NPC = *It;
        if (!NPC || NPC->GetPersistentId().IsNone())
        {
            continue;
        }
        if (NPCsById.Contains(NPC->GetPersistentId()))
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS load aborted: duplicate live NPC PersistentId '%s'."),
                *NPC->GetPersistentId().ToString());
            return false;
        }
        NPCsById.Add(NPC->GetPersistentId(), NPC);
    }

    for (const FOmnisSavedCharacterState& SavedCharacter : Save->Characters)
    {
        if (SavedCharacter.PersistentId.IsNone())
        {
            UE_LOG(LogTemp, Error, TEXT("OMNIS load aborted: save contains an empty PersistentId."));
            return false;
        }
        if (SavedCharacter.bIsPlayer)
        {
            if (!UGameplayStatics::GetPlayerCharacter(World, 0))
            {
                UE_LOG(LogTemp, Error, TEXT("OMNIS load aborted: player pawn is not ready."));
                return false;
            }
        }
        else if (!NPCsById.Contains(SavedCharacter.PersistentId))
        {
            UE_LOG(LogTemp, Error,
                TEXT("OMNIS load aborted: NPC '%s' is not present in the loaded world."),
                *SavedCharacter.PersistentId.ToString());
            return false;
        }
    }

    ImportPersistentInts(Save->PersistentInts);

    if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
    {
        Cycle->RestoreCycleState(Save->CycleState);
    }
    if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
    {
        Nexus->ImportProgression(Save->NexusProgression);
    }
    if (UOmnisQuestSubsystem* Quests = GameInstance->GetSubsystem<UOmnisQuestSubsystem>())
    {
        Quests->ImportProgress(Save->QuestProgress);
    }
    if (UOmnisConsequenceSubsystem* Consequences =
        GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
    {
        Consequences->ImportConsequences(Save->ActiveConsequences);
    }
    if (UOmnisQuestSubsystem* Quests = GameInstance->GetSubsystem<UOmnisQuestSubsystem>())
    {
        Quests->RestoreFailureConsequences();
    }
    if (UOmnisReputationSubsystem* Reputation =
        GameInstance->GetSubsystem<UOmnisReputationSubsystem>())
    {
        Reputation->ImportReputation(Save->ReputationValues);
    }
    if (UOmnisFactionSubsystem* Factions =
        GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
    {
        Factions->ImportStandings(Save->FactionStandings);
    }
    if (UOmnisWorldStateSubsystem* WorldState =
        World->GetSubsystem<UOmnisWorldStateSubsystem>())
    {
        WorldState->ImportStates(Save->WorldStateValues);
    }
    if (UOmnisTimeSubsystem* Time = World->GetSubsystem<UOmnisTimeSubsystem>())
    {
        Time->RestoreElapsedSeconds(Save->WorldElapsedSeconds);
        Time->SetTimeScale(Save->WorldTimeScale);
    }

    for (const FOmnisSavedCharacterState& SavedCharacter : Save->Characters)
    {
        if (SavedCharacter.PersistentId.IsNone())
        {
            continue;
        }

        if (SavedCharacter.bIsPlayer)
        {
            if (AOmnisPlayerCharacter* Player =
                Cast<AOmnisPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0)))
            {
                Player->SetActorTransform(
                    SavedCharacter.Transform, false, nullptr, ETeleportType::TeleportPhysics);
                if (Player->CharacterState)
                {
                    Player->CharacterState->ImportSnapshot(SavedCharacter.LifeState);
                }
                if (Player->Health)
                {
                    Player->Health->RestoreSavedState(
                        SavedCharacter.Health,
                        SavedCharacter.MaxHealth,
                        !SavedCharacter.LifeState.bAlive);
                }
                if (Player->Inventory)
                {
                    Player->Inventory->ImportItems(SavedCharacter.InventoryItems);
                }
                if (Player->Equipment)
                {
                    Player->Equipment->ImportEquipment(SavedCharacter.EquippedItems);
                }
                if (Player->Abilities)
                {
                    Player->Abilities->ImportCooldowns(SavedCharacter.AbilityCooldowns);
                }
            }
            continue;
        }

        if (AOmnisNPCCharacter** FoundNPC = NPCsById.Find(SavedCharacter.PersistentId))
        {
            (*FoundNPC)->RestorePersistentState(
                SavedCharacter.Transform,
                SavedCharacter.LifeState);
            if ((*FoundNPC)->Health)
            {
                (*FoundNPC)->Health->RestoreSavedState(
                    SavedCharacter.Health,
                    SavedCharacter.MaxHealth,
                    !SavedCharacter.LifeState.bAlive);
            }
        }
    }

    if (UOmnisCharacterLifeSubsystem* Life =
        World->GetSubsystem<UOmnisCharacterLifeSubsystem>())
    {
        Life->RebaseRegisteredNPCsToCurrentTime();
    }

    for (TActorIterator<AOmnisReplicatedWorldStateActor> It(World); It; ++It)
    {
        It->RefreshNarrativeState();
        break;
    }

    return true;
}
