#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisGameplayEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOmnisGameplayEvent, FName, EventId, AActor*, Instigator);

UCLASS()
class OMNIS_API UOmnisGameplayEventSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|GameplayEvents")
    void Emit(FName EventId, AActor* Instigator);

    UPROPERTY(BlueprintAssignable, Category="Omnis|GameplayEvents")
    FOnOmnisGameplayEvent OnEvent;
};
