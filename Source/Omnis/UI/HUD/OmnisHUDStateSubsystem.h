#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisHUDStateSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnisNotification, FText, Message);

UCLASS()
class OMNIS_API UOmnisHUDStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void PushNotification(FText Message);

    UPROPERTY(BlueprintAssignable, Category="Omnis|UI")
    FOnOmnisNotification OnNotification;
};
