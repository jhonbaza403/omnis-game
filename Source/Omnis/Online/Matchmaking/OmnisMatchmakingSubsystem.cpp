#include "Online/Matchmaking/OmnisMatchmakingSubsystem.h"

#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UOmnisMatchmakingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface())
        {
            FindSessionsDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
                FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::HandleFindSessionsComplete)
            );
            JoinSessionDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
                FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleJoinSessionComplete)
            );
        }
    }
}

void UOmnisMatchmakingSubsystem::Deinitialize()
{
    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface())
        {
            if (FindSessionsDelegateHandle.IsValid())
            {
                Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegateHandle);
            }
            if (JoinSessionDelegateHandle.IsValid())
            {
                Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);
            }
        }
    }

    SearchSettings.Reset();
    Super::Deinitialize();
}

bool UOmnisMatchmakingSubsystem::FindSessions(FName NewPlaylistId, int32 MaxResults)
{
    if (bSearching || bJoining || NewPlaylistId.IsNone() || MaxResults <= 0)
    {
        return false;
    }

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
    const IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
    if (!Identity.IsValid() || !Sessions.IsValid() ||
        Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    SearchSettings = MakeShared<FOnlineSessionSearch>();
    SearchSettings->bIsLanQuery = false;
    SearchSettings->MaxSearchResults = FMath::Clamp(MaxResults, 1, 100);
    SearchSettings->TimeoutInSeconds = 10.0f;
    SearchSettings->QuerySettings.Set(
        FName(TEXT("OMNIS_GAME")),
        true,
        EOnlineComparisonOp::Equals
    );
    SearchSettings->QuerySettings.Set(
        FName(TEXT("OMNIS_PLAYLIST")),
        NewPlaylistId.ToString(),
        EOnlineComparisonOp::Equals
    );

    PlaylistId = NewPlaylistId;
    bSearching = Sessions->FindSessions(0, SearchSettings.ToSharedRef());
    if (!bSearching)
    {
        PlaylistId = NAME_None;
        SearchSettings.Reset();
    }

    return bSearching;
}

bool UOmnisMatchmakingSubsystem::JoinSessionByIndex(int32 ResultIndex)
{
    if (bSearching || bJoining || !SearchSettings.IsValid() ||
        !SearchSettings->SearchResults.IsValidIndex(ResultIndex))
    {
        return false;
    }

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
    const IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
    if (!Identity.IsValid() || !Sessions.IsValid() ||
        Identity->GetLoginStatus(0) != ELoginStatus::LoggedIn)
    {
        return false;
    }

    static const FName JoinSessionName(TEXT("OMNIS_MATCH"));
    bJoining = Sessions->JoinSession(0, JoinSessionName, SearchSettings->SearchResults[ResultIndex]);
    return bJoining;
}

bool UOmnisMatchmakingSubsystem::CancelFind()
{
    if (!bSearching)
    {
        return false;
    }

    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface())
        {
            return Sessions->CancelFindSessions();
        }
    }

    return false;
}

void UOmnisMatchmakingSubsystem::Enqueue(FName NewPlaylistId)
{
    FindSessions(NewPlaylistId);
}

void UOmnisMatchmakingSubsystem::Dequeue()
{
    CancelFind();
    PlaylistId = NAME_None;
}

int32 UOmnisMatchmakingSubsystem::GetFoundSessionCount() const
{
    return SearchSettings.IsValid() ? SearchSettings->SearchResults.Num() : 0;
}

FOmnisSearchResultView UOmnisMatchmakingSubsystem::GetSearchResult(int32 ResultIndex) const
{
    FOmnisSearchResultView View;

    if (!SearchSettings.IsValid() || !SearchSettings->SearchResults.IsValidIndex(ResultIndex))
    {
        return View;
    }

    const FOnlineSessionSearchResult& Result = SearchSettings->SearchResults[ResultIndex];
    View.SessionId = Result.GetSessionIdStr();

    if (Result.Session.IsValid())
    {
        View.OpenConnections = Result.Session->NumOpenPublicConnections;
        View.TotalConnections = Result.Session->NumPublicConnections;
        View.bUsesPresence = Result.Session->SessionSettings.bUsesPresence;
    }

    return View;
}

void UOmnisMatchmakingSubsystem::HandleFindSessionsComplete(bool bWasSuccessful)
{
    bSearching = false;
    OnSearchComplete.Broadcast(bWasSuccessful);
}

void UOmnisMatchmakingSubsystem::HandleJoinSessionComplete(
    FName SessionName,
    EOnJoinSessionCompleteResult::Type Result)
{
    bJoining = false;

    FString ConnectString;
    bool bSuccess = false;

    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
        {
            if (IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface())
            {
                bSuccess = Sessions->GetResolvedConnectString(
                    SessionName,
                    ConnectString,
                    NAME_GamePort
                );
            }
        }

        if (bSuccess)
        {
            if (UWorld* World = GetWorld())
            {
                if (APlayerController* PlayerController = World->GetFirstPlayerController())
                {
                    PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
                }
            }
        }
    }

    OnJoinComplete.Broadcast(bSuccess, ConnectString);
}
