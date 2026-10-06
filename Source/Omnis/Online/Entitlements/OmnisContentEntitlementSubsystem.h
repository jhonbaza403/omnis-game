#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisContentEntitlementSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisContentEntitlementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Content")
    bool RefreshEntitlements(const FString& Namespace = TEXT("omnis"));

    UFUNCTION(BlueprintPure, Category="Omnis|Content")
    bool IsContentEntitled(const FString& ItemId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Content")
    void ClearCachedEntitlements();

private:
    UPROPERTY()
    TSet<FString> CachedEntitlementItemIds;
};
