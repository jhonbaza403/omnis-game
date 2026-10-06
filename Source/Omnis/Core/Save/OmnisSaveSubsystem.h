#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisSaveSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static constexpr int32 MinSlot = 1;
    static constexpr int32 MaxSlot = 10;

    UFUNCTION(BlueprintCallable, Category="Omnis|Save")
    bool SaveToSlot(int32 Slot = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Save")
    bool LoadFromSlot(int32 Slot = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Save")
    bool DeleteSlot(int32 Slot = 1);

    UFUNCTION(BlueprintPure, Category="Omnis|Save")
    bool HasSave(int32 Slot = 1) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Save")
    int32 GetLastUsedSlot() const { return LastUsedSlot; }

private:
    FString MakeSlotName(int32 Slot) const;
    int32 NormalizeSlot(int32 Slot) const;

    UPROPERTY()
    int32 LastUsedSlot = 1;
};
