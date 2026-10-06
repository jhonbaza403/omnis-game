#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOmnisSessionOperationCompleted, FName, SessionName, bool, bSuccess);

UCLASS()
class OMNIS_API UOmnisSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    bool CreateSession(FName NewSessionId, int32 MaxPlayers = 16, FName NewPlaylistId = NAME_None);

    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    bool LeaveSession();

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    bool IsSessionActive() const { return bSessionActive; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    bool IsSessionOperationPending() const { return bOperationPending; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    FName GetSessionId() const { return SessionId; }

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    FName GetPlaylistId() const { return PlaylistId; }

    UPROPERTY(BlueprintAssignable, Category="Omnis|Multiplayer")
    FOmnisSessionOperationCompleted OnSessionCreated;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Multiplayer")
    FOmnisSessionOperationCompleted OnSessionDestroyed;

private:
    void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    UPROPERTY()
    bool bSessionActive = false;

    UPROPERTY()
    bool bOperationPending = false;

    UPROPERTY()
    FName SessionId;

    UPROPERTY()
    FName PlaylistId;

    UPROPERTY()
    int32 MaxPlayers = 0;

    FDelegateHandle CreateSessionDelegateHandle;
    FDelegateHandle DestroySessionDelegateHandle;
};
