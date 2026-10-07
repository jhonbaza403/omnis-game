#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Online/HTTPTypes.h"
#include "OmnisOnlineServiceSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisOnlineServiceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    using FOmnisHttpCompletion = TFunction<void(bool bSucceeded, int32 StatusCode, const FString& ResponseBody)>;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetServiceAvailable(FName ServiceId, bool bAvailable);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsServiceAvailable(FName ServiceId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetPlayerId(const FString& NewPlayerId);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetPlayerId() const { return PlayerId; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetGatewayBaseUrl() const { return GatewayBaseUrl; }

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void RefreshGatewayHealth();

    void SendRequest(
        const FString& Verb,
        const FString& Path,
        const FString& JsonBody,
        FOmnisHttpCompletion Completion);

private:
    FString BuildApiUrl(const FString& Path) const;

    UPROPERTY()
    TMap<FName, bool> Services;

    UPROPERTY()
    FString GatewayBaseUrl;

    UPROPERTY()
    FString PlayerId;
};
