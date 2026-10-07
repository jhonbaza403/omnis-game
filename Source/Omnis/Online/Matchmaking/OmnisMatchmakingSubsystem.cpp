#include "Online/Matchmaking/OmnisMatchmakingSubsystem.h"

#include "Dom/JsonObject.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Online/Services/OmnisOnlineServiceSubsystem.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
    FString SerializeJson(const TSharedRef<FJsonObject>& Object)
    {
        FString Output;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
        FJsonSerializer::Serialize(Object, Writer);
        return Output;
    }

    FString EncodeQueryValue(const FString& Value)
    {
        return FGenericPlatformHttp::UrlEncode(Value);
    }
}

void UOmnisMatchmakingSubsystem::Enqueue(FName NewPlaylistId)
{
    if (NewPlaylistId.IsNone())
    {
        return;
    }

    PlaylistId = NewPlaylistId;
    bQueued = false;

    UOmnisOnlineServiceSubsystem* Online = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UOmnisOnlineServiceSubsystem>()
        : nullptr;

    if (!Online)
    {
        return;
    }

    const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("playlist"), PlaylistId.ToString());
    const TWeakObjectPtr<UOmnisOnlineServiceSubsystem> WeakOnline(Online);

    Online->SendRequest(
        TEXT("POST"),
        TEXT("/api/matchmaking/enqueue"),
        SerializeJson(Payload),
        [WeakThis = TWeakObjectPtr<UOmnisMatchmakingSubsystem>(this), WeakOnline](
            bool bSucceeded,
            int32 StatusCode,
            const FString&)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            WeakThis->bQueued = bSucceeded && StatusCode == 202;

            if (WeakOnline.IsValid())
            {
                WeakOnline->SetServiceAvailable(TEXT("matchmaking"), bSucceeded);
            }
        });
}

void UOmnisMatchmakingSubsystem::Dequeue()
{
    if (PlaylistId.IsNone())
    {
        bQueued = false;
        return;
    }

    const FString Playlist = PlaylistId.ToString();
    UOmnisOnlineServiceSubsystem* Online = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UOmnisOnlineServiceSubsystem>()
        : nullptr;

    if (!Online)
    {
        return;
    }

    const FString Path = FString::Printf(
        TEXT("/api/matchmaking/queue?playlist=%s"),
        *EncodeQueryValue(Playlist));

    const TWeakObjectPtr<UOmnisOnlineServiceSubsystem> WeakOnline(Online);

    Online->SendRequest(
        TEXT("DELETE"),
        Path,
        TEXT(""),
        [WeakThis = TWeakObjectPtr<UOmnisMatchmakingSubsystem>(this), WeakOnline](
            bool bSucceeded,
            int32,
            const FString&)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            WeakThis->bQueued = false;
            WeakThis->PlaylistId = NAME_None;

            if (WeakOnline.IsValid())
            {
                WeakOnline->SetServiceAvailable(TEXT("matchmaking"), bSucceeded);
            }
        });
}

void UOmnisMatchmakingSubsystem::RefreshStatus()
{
    if (PlaylistId.IsNone())
    {
        bQueued = false;
        return;
    }

    UOmnisOnlineServiceSubsystem* Online = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UOmnisOnlineServiceSubsystem>()
        : nullptr;

    if (!Online)
    {
        return;
    }

    const FString Path = FString::Printf(
        TEXT("/api/matchmaking/status?playlist=%s"),
        *EncodeQueryValue(PlaylistId.ToString()));

    const TWeakObjectPtr<UOmnisOnlineServiceSubsystem> WeakOnline(Online);

    Online->SendRequest(
        TEXT("GET"),
        Path,
        TEXT(""),
        [WeakThis = TWeakObjectPtr<UOmnisMatchmakingSubsystem>(this), WeakOnline](
            bool bSucceeded,
            int32,
            const FString& ResponseBody)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            bool bServerQueued = false;
            if (bSucceeded)
            {
                TSharedPtr<FJsonObject> Root;
                const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
                if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
                {
                    Root->TryGetBoolField(TEXT("queued"), bServerQueued);
                }
            }

            WeakThis->bQueued = bSucceeded && bServerQueued;

            if (WeakOnline.IsValid())
            {
                WeakOnline->SetServiceAvailable(TEXT("matchmaking"), bSucceeded);
            }
        });
}
