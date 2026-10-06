#include "Multiplayer/Session/OmnisSessionSubsystem.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UOmnisSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface())
        {
            CreateSessionDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
                FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleCreateSessionComplete)
            );
            DestroySessionDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
                FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleDestroySessionComplete)
            );
        }
    }
}

void UOmnisSessionSubsystem::Deinitialize()
{
    if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface())
        {
            if (CreateSessionDelegateHandle.IsValid())
            {
                Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
            }
            if (DestroySessionDelegateHandle.IsValid())
            {
                Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
            }
        }
    }

    Super::Deinitialize();
}

bool UOmnisSessionSubsystem::CreateSession(FName NewSessionId, int32 InMaxPlayers, FName NewPlaylistId)
{
    if (bOperationPending || bSessionActive || NewSessionId.IsNone() || InMaxPlayers <= 0)
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

    const FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
    if (!UserId.IsValid())
    {
        return false;
    }

    FOnlineSessionSettings Settings;
    Settings.NumPublicConnections = InMaxPlayers;
    Settings.NumPrivateConnections = 0;
    Settings.bIsLANMatch = false;
    Settings.bShouldAdvertise = true;
    Settings.bAllowJoinInProgress = true;
    Settings.bAllowInvites = true;
    Settings.bAllowJoinViaPresence = true;
    Settings.bUsesPresence = true;
    Settings.bUsesStats = true;
    Settings.bUseLobbiesIfAvailable = true;
    Settings.bUseLobbiesVoiceChatIfAvailable = true;
    Settings.BuildUniqueId = 1;

    Settings.Set(
        FName(TEXT("OMNIS_GAME")),
        true,
        EOnlineDataAdvertisementType::ViaOnlineService
    );

    const FName SafePlaylist = NewPlaylistId.IsNone() ? FName(TEXT("default")) : NewPlaylistId;
    Settings.Set(
        FName(TEXT("OMNIS_PLAYLIST")),
        SafePlaylist.ToString(),
        EOnlineDataAdvertisementType::ViaOnlineService
    );

    bOperationPending = Sessions->CreateSession(*UserId, NewSessionId, Settings);
    if (bOperationPending)
    {
        SessionId = NewSessionId;
        PlaylistId = SafePlaylist;
        MaxPlayers = InMaxPlayers;
    }

    return bOperationPending;
}

bool UOmnisSessionSubsystem::LeaveSession()
{
    if (bOperationPending || !bSessionActive || SessionId.IsNone())
    {
        return false;
    }

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem)
    {
        return false;
    }

    const IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
    if (!Sessions.IsValid())
    {
        return false;
    }

    bOperationPending = Sessions->DestroySession(
        SessionId,
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::HandleDestroySessionComplete)
    );
    return bOperationPending;
}

void UOmnisSessionSubsystem::HandleCreateSessionComplete(FName NewSessionName, bool bWasSuccessful)
{
    if (NewSessionName != SessionId)
    {
        return;
    }

    bOperationPending = false;
    bSessionActive = bWasSuccessful;

    if (!bWasSuccessful)
    {
        SessionId = NAME_None;
        PlaylistId = NAME_None;
        MaxPlayers = 0;
    }

    OnSessionCreated.Broadcast(NewSessionName, bWasSuccessful);
}

void UOmnisSessionSubsystem::HandleDestroySessionComplete(FName NewSessionName, bool bWasSuccessful)
{
    if (NewSessionName != SessionId)
    {
        return;
    }

    bOperationPending = false;

    if (bWasSuccessful)
    {
        bSessionActive = false;
        SessionId = NAME_None;
        PlaylistId = NAME_None;
        MaxPlayers = 0;
    }

    OnSessionDestroyed.Broadcast(NewSessionName, bWasSuccessful);
}
