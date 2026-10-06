#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisSessionSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    bool CreateSession(FName SessionId, int32 MaxPlayers = 16);

    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    void LeaveSession();

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    bool IsSessionActive() const { return bSessionActive; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    FName GetSessionId() const { return SessionId; }

private:
    UPROPERTY()
    bool bSessionActive = false;

    UPROPERTY()
    FName SessionId;

    UPROPERTY()
    int32 MaxPlayers = 0;
};
