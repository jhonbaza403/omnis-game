#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisPersistenceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisPersistenceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Persistence")
    void SetPersistentInt(FName Key, int32 Value);

    UFUNCTION(BlueprintPure, Category="Omnis|Persistence")
    int32 GetPersistentInt(FName Key, int32 DefaultValue = 0) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Persistence")
    bool SaveCurrentGame(const FString& SlotName = TEXT("Omnis_Main"));

    UFUNCTION(BlueprintCallable, Category="Omnis|Persistence")
    bool LoadCurrentGame(const FString& SlotName = TEXT("Omnis_Main"));

    UFUNCTION(BlueprintPure, Category="Omnis|Persistence")
    TMap<FName, int32> ExportPersistentInts() const { return PersistentInts; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Persistence")
    void ImportPersistentInts(const TMap<FName, int32>& Values);

private:
    UPROPERTY(SaveGame)
    TMap<FName, int32> PersistentInts;
};
