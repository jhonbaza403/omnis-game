#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAuthenticationSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisAuthState : uint8
{
    SignedOut,
    LocalIdentity,
    SignedIn
};

UCLASS()
class OMNIS_API UOmnisAuthenticationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetExternalIdentity(FString UserId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SetAccessToken(FString NewAccessToken);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void SignOut();

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    EOmnisAuthState GetState() const { return State; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetUserId() const { return UserId; }

    FString GetAccessToken() const { return AccessToken; }

private:
    UPROPERTY()
    FString UserId;

    UPROPERTY(Transient)
    FString AccessToken;

    UPROPERTY()
    EOmnisAuthState State = EOmnisAuthState::SignedOut;
};
