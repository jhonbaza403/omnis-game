#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOmnisEventRaised, FName, EventId, int32, Value);

UCLASS()
class OMNIS_API UOmnisEventSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Events")
    void RaiseEvent(FName EventId, int32 Value = 0);

    UPROPERTY(BlueprintAssignable, Category="Omnis|Events")
    FOnOmnisEventRaised OnEventRaised;
};
