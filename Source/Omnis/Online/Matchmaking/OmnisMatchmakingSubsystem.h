#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisMatchmakingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOmnisMatchmakingComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOmnisJoinComplete, bool, bSuccess, FString, ConnectString);

USTRUCT(BlueprintType)
struct FOmnisSearchResultView
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Online")
    FString SessionId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Online")
    int32 OpenConnections = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Online")
    int32 TotalConnections = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Online")
    bool bUsesPresence = false;
};

UCLASS()
class OMNIS_API UOmnisMatchmakingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool FindSessions(FName PlaylistId, int32 MaxResults = 50);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool JoinSessionByIndex(int32 ResultIndex);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool CancelFind();

    UFUNCTION(BlueprintCallable, Category="Omnis|Online|Compatibility")
    void Enqueue(FName PlaylistId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online|Compatibility")
    void Dequeue();

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsSearching() const { return bSearching; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsJoining() const { return bJoining; }

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    int32 GetFoundSessionCount() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    FOmnisSearchResultView GetSearchResult(int32 ResultIndex) const;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Online")
    FOmnisMatchmakingComplete OnSearchComplete;

    UPROPERTY(BlueprintAssignable, Category="Omnis|Online")
    FOmnisJoinComplete OnJoinComplete;

private:
    void HandleFindSessionsComplete(bool bWasSuccessful);
    void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    UPROPERTY()
    bool bSearching = false;

    UPROPERTY()
    bool bJoining = false;

    UPROPERTY()
    FName PlaylistId;

    TSharedPtr<FOnlineSessionSearch> SearchSettings;

    FDelegateHandle FindSessionsDelegateHandle;
    FDelegateHandle JoinSessionDelegateHandle;
};
