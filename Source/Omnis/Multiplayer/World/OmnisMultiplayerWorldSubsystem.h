#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisMultiplayerWorldSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisMultiplayerWorldSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    void SetWorldId(FName NewWorldId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    void RefreshWorldState();

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    FName GetWorldId() const { return WorldId; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    int32 GetWorldRevision() const { return WorldRevision; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    int32 GetCycleIndex() const { return CycleIndex; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    FString GetEraName() const { return EraName; }

private:
    UPROPERTY()
    FName WorldId;

    UPROPERTY()
    int32 WorldRevision = 0;

    UPROPERTY()
    int32 CycleIndex = 0;

    UPROPERTY()
    FString EraName;
};
