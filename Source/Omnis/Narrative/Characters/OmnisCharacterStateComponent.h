#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    FName EventId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    FName RelatedCharacterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Omnis|Life")
    int32 Importance = 0;
};

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisCharacterStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisCharacterStateComponent();

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

    const TArray<FOmnisCharacterMemoryEntry>& GetMemories() const { return Memories; }

private:
    UPROPERTY()
    bool bAlive = true;

    UPROPERTY()
    TMap<FName, int32> Relationships;

    UPROPERTY()
    TArray<FOmnisCharacterMemoryEntry> Memories;

    UPROPERTY()
    EOmnisCharacterLifeGoal CurrentGoal = EOmnisCharacterLifeGoal::Idle;

    // Hunger and social need rise over time; energy falls while active.
    UPROPERTY()
    float Hunger = 0.0f;

    UPROPERTY()
    float Energy = 100.0f;

    UPROPERTY()
    float SocialNeed = 0.0f;
};
