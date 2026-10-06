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
    void SignOut();

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    EOmnisAuthState GetState() const { return State; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FString GetUserId() const { return UserId; }

private:
    UPROPERTY()
    FString UserId;

    UPROPERTY()
    EOmnisAuthState State = EOmnisAuthState::SignedOut;
};
