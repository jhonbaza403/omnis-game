#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisCloudPersistenceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnOmnisCloudRequestCompleted,
    FName, Operation,
    bool, bSuccess,
    int32, StatusCode,
    FString, Payload
);

UCLASS()
class OMNIS_API UOmnisCloudPersistenceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetApiBaseUrl(FString NewBaseUrl);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetAccessToken(FString NewAccessToken);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool SaveSlotJson(int32 Slot, const FString& StateJson, int32 SaveVersion = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool LoadSlotJson(int32 Slot);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool DeleteSlot(int32 Slot);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool SubmitEvent(FName EventType, const FString& PayloadJson, int32 EventVersion = 1);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsConfigured() const { return !ApiBaseUrl.IsEmpty() && !AccessToken.IsEmpty(); }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Omnis|Online")
    FString ApiBaseUrl = TEXT("https://omnis-game.bazajhon.workers.dev");

    UPROPERTY(BlueprintAssignable, Category="Omnis|Online")
    FOnOmnisCloudRequestCompleted OnRequestCompleted;

private:
    bool IsValidSlot(int32 Slot) const;
    bool IsValidJsonObject(const FString& Json) const;

    void ExecuteRequest(FName Operation, const FString& Method, const FString& Path, const FString& Body);

    UPROPERTY(Transient)
    FString AccessToken;
};
