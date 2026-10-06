#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisHUDStateSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnisNotification, FText, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnisActiveUserChanged, FText, DisplayName);

UCLASS()
class OMNIS_API UOmnisHUDStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void PushNotification(FText Message);

    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetActiveUserDisplayName(FString Name);

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    FText GetActiveUserDisplayName() const { return ActiveUserDisplayName; }

    UPROPERTY(BlueprintAssignable, Category="Omnis|UI")
    FOnOmnisNotification OnNotification;

    UPROPERTY(BlueprintAssignable, Category="Omnis|UI")
    FOnOmnisActiveUserChanged OnActiveUserChanged;

private:
    UPROPERTY()
    FText ActiveUserDisplayName;
};
