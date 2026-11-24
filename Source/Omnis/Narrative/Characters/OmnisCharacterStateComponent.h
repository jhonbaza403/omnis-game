#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmnisCharacterStateComponent.generated.h"

UENUM(BlueprintType)
enum class EOmnisCharacterLifeGoal : uint8
{
    Idle,
    Work,
    Eat,
    Rest,
    Socialize,
    Explore,
    SeekSafety
};

USTRUCT(BlueprintType)
struct FOmnisCharacterMemoryEntry
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    FName EventId;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    FName RelatedCharacterId;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    int32 Importance = 0;
};

USTRUCT(BlueprintType)
struct FOmnisCharacterLifeSnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    bool bAlive = true;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    TMap<FName, int32> Relationships;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    TArray<FOmnisCharacterMemoryEntry> Memories;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    EOmnisCharacterLifeGoal CurrentGoal = EOmnisCharacterLifeGoal::Idle;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    float Hunger = 0.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    float Energy = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Life")
    float SocialNeed = 0.0f;
};

USTRUCT(BlueprintType)
struct FOmnisReplicatedRelationship
{
    GENERATED_BODY()

    UPROPERTY()
    FName SubjectId;

    UPROPERTY()
    int32 Value = 0;
};

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisCharacterStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisCharacterStateComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void SetAlive(bool bNewAlive);

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void SetRelationship(FName SubjectId, int32 Value);

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void AdjustRelationship(FName SubjectId, int32 Delta);

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void RecordMemory(FName EventId, FName RelatedCharacterId, int32 Importance);

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void SetCurrentGoal(EOmnisCharacterLifeGoal NewGoal);

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void SimulateNeeds(float DeltaSeconds, EOmnisCharacterLifeGoal Goal, bool bReachedGoal);

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    bool IsAlive() const { return bAlive; }

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    int32 GetRelationship(FName SubjectId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    EOmnisCharacterLifeGoal GetCurrentGoal() const { return CurrentGoal; }

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    float GetHunger() const { return Hunger; }

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    float GetEnergy() const { return Energy; }

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    float GetSocialNeed() const { return SocialNeed; }

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    FOmnisCharacterLifeSnapshot ExportSnapshot() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    void ImportSnapshot(const FOmnisCharacterLifeSnapshot& Snapshot);

    const TArray<FOmnisCharacterMemoryEntry>& GetMemories() const { return Memories; }

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    bool HasMemory(FName EventId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    bool RemoveMemory(FName EventId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Life")
    int32 RemoveMemoriesByPrefix(const FString& EventPrefix);

    UFUNCTION(BlueprintPure, Category="Omnis|Life")
    bool HasMemoryPrefix(const FString& EventPrefix) const;

private:
    UFUNCTION()
    void OnRep_Alive();

    UFUNCTION()
    void OnRep_Relationships();

    void SyncRelationshipsForReplication();
    bool HasMutationAuthority() const;

    UPROPERTY(SaveGame, ReplicatedUsing=OnRep_Alive)
    bool bAlive = true;

    UPROPERTY(SaveGame)
    TMap<FName, int32> Relationships;

    UPROPERTY(ReplicatedUsing=OnRep_Relationships)
    TArray<FOmnisReplicatedRelationship> ReplicatedRelationships;

    UPROPERTY(SaveGame, Replicated)
    TArray<FOmnisCharacterMemoryEntry> Memories;

    UPROPERTY(SaveGame, Replicated)
    EOmnisCharacterLifeGoal CurrentGoal = EOmnisCharacterLifeGoal::Idle;

    UPROPERTY(SaveGame, Replicated)
    float Hunger = 0.0f;

    UPROPERTY(SaveGame, Replicated)
    float Energy = 100.0f;

    UPROPERTY(SaveGame, Replicated)
    float SocialNeed = 0.0f;
};
