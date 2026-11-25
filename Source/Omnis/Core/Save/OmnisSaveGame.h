#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/Cycle/OmnisCycleTypes.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Gameplay/Abilities/OmnisAbilityComponent.h"
#include "Gameplay/Equipment/OmnisEquipmentComponent.h"
#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Narrative/Quests/OmnisQuestSubsystem.h"
#include "World/Economy/OmnisEconomySubsystem.h"
#include "World/Destruction/OmnisStructuralDamageComponent.h"
#include "Narrative/Factions/OmnisFactionSubsystem.h"
#include "Gameplay/Vehicle/OmnisVehicleConditionComponent.h"
#include "World/Terrain/OmnisVenezuelaTerrainSubsystem.h"
#include "OmnisSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FOmnisSavedCharacterState
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    FName PersistentId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    bool bIsPlayer = false;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    FTransform Transform = FTransform::Identity;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    FOmnisCharacterLifeSnapshot LifeState;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    float Health = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    float MaxHealth = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    TArray<FOmnisInventoryEntry> InventoryItems;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    TArray<FOmnisEquipmentEntry> EquippedItems;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    TArray<FOmnisSavedAbilityCooldown> AbilityCooldowns;
};

USTRUCT(BlueprintType)
struct FOmnisSavedVehicleState
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    FName PersistentId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    FTransform Transform = FTransform::Identity;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Save")
    FOmnisVehicleConditionSnapshot Condition;
};

UCLASS()
class OMNIS_API UOmnisSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    int32 SchemaVersion = 7;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    FDateTime SavedAtUtc;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    FOmnisCycleState CycleState;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    FOmnisNexusProgression NexusProgression;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    int32 PermanentUpgradePoints = 0;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    float WorldElapsedSeconds = 0.0f;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    float WorldTimeScale = 1.0f;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOmnisQuestProgressSnapshot> QuestProgress;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FName> ActiveConsequences;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOmnisSavedCharacterState> Characters;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TMap<FName, int32> WorldStateValues;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TMap<FName, int32> ReputationValues;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TMap<FName, int32> FactionStandings;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TMap<FName, int32> PersistentInts;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    FOmnisEconomySnapshot EconomyState;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOmnisStructuralDamageSnapshot> StructuralDamageSnapshots;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOmnisTerrainDeformationSnapshot> TerrainDeformationSnapshots;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOmnisFactionTerritorySnapshot> FactionTerritoryStates;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category="Save")
    TArray<FOmnisSavedVehicleState> Vehicles;
};
