#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisMenuStateSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisMenuStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void OpenMenu(FName MenuId);

    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void CloseMenu();

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    FName GetOpenMenu() const { return OpenMenuId; }

private:
    UPROPERTY()
    FName OpenMenuId;
};
