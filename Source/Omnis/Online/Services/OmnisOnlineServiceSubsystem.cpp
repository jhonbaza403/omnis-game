#include "Online/Services/OmnisOnlineServiceSubsystem.h"

#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformMisc.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
    FString SerializeObject(const TSharedRef<FJsonObject>& Object)
    {
        FString Output;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
        FJsonSerializer::Serialize(Object, Writer);
        return Output;
    }
}

void UOmnisOnlineServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    GatewayBaseUrl = FPlatformMisc::GetEnvironmentVariable(TEXT("OMNIS_BACKEND_URL"));
    if (GatewayBaseUrl.IsEmpty() && GConfig)
    {
        GConfig->GetString(TEXT("Omnis.Online"), TEXT("GatewayUrl"), GatewayBaseUrl, GGameIni);
    }

    if (GatewayBaseUrl.IsEmpty())
    {
        GatewayBaseUrl = TEXT("http://127.0.0.1:8080");
    }

    while (GatewayBaseUrl.EndsWith(TEXT("/")))
    {
        GatewayBaseUrl.RemoveAt(GatewayBaseUrl.Len() - 1);
    }

    PlayerId = FPlatformMisc::GetEnvironmentVariable(TEXT("OMNIS_PLAYER_ID"));
    ServiceToken = FPlatformMisc::GetEnvironmentVariable(TEXT("OMNIS_SERVICE_TOKEN"));

    SetServiceAvailable(TEXT("gateway"), false);
    RefreshGatewayHealth();
}

void UOmnisOnlineServiceSubsystem::Deinitialize()
{
    Services.Empty();
    GatewayBaseUrl.Empty();
    PlayerId.Empty();
    ServiceToken.Empty();

    Super::Deinitialize();
}

void UOmnisOnlineServiceSubsystem::SetServiceAvailable(FName ServiceId, bool bAvailable)
{
    if (!ServiceId.IsNone())
    {
        Services.Add(ServiceId, bAvailable);
    }
}

bool UOmnisOnlineServiceSubsystem::IsServiceAvailable(FName ServiceId) const
{
    return Services.FindRef(ServiceId);
}

void UOmnisOnlineServiceSubsystem::SetPlayerId(const FString& NewPlayerId)
{
    PlayerId = NewPlayerId.TrimStartAndEnd();
}

void UOmnisOnlineServiceSubsystem::RefreshGatewayHealth()
{
    SendRequest(
        TEXT("GET"),
        TEXT("/healthz"),
        TEXT(""),
        [WeakThis = TWeakObjectPtr<UOmnisOnlineServiceSubsystem>(this)](
            bool bSucceeded,
            int32 StatusCode,
            const FString&)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            WeakThis->SetServiceAvailable(TEXT("gateway"), bSucceeded && StatusCode == 200);
        });
}

void UOmnisOnlineServiceSubsystem::GetWorldCycle(FOmnisHttpCompletion Completion)
{
    SendRequest(TEXT("GET"), TEXT("/api/world/cycle"), TEXT(""), MoveTemp(Completion));
}

void UOmnisOnlineServiceSubsystem::AdvanceWorldCycle(
    const FString& Era,
    const FString& Reason,
    FOmnisHttpCompletion Completion)
{
    const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    if (!Era.IsEmpty())
    {
        Payload->SetStringField(TEXT("era"), Era);
    }
    if (!Reason.IsEmpty())
    {
        Payload->SetStringField(TEXT("reason"), Reason);
    }

    SendRequest(
        TEXT("POST"),
        TEXT("/api/world/cycle/advance"),
        SerializeObject(Payload),
        MoveTemp(Completion));
}

void UOmnisOnlineServiceSubsystem::RecordWorldEvent(
    const FString& EventType,
    const FString& EventPlayerId,
    const FString& JsonPayload,
    FOmnisHttpCompletion Completion)
{
    const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("event_type"), EventType);

    if (!EventPlayerId.IsEmpty())
    {
        Payload->SetStringField(TEXT("player_id"), EventPlayerId);
    }

    if (!JsonPayload.IsEmpty())
    {
        TSharedPtr<FJsonObject> EventPayload;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonPayload);
        if (FJsonSerializer::Deserialize(Reader, EventPayload) && EventPayload.IsValid())
        {
            Payload->SetObjectField(TEXT("payload"), EventPayload);
        }
    }

    SendRequest(
        TEXT("POST"),
        TEXT("/api/world/events"),
        SerializeObject(Payload),
        MoveTemp(Completion));
}

void UOmnisOnlineServiceSubsystem::SendRequest(
    const FString& Verb,
    const FString& Path,
    const FString& JsonBody,
    FOmnisHttpCompletion Completion)
{
    if (GatewayBaseUrl.IsEmpty())
    {
        if (Completion)
        {
            Completion(false, 0, TEXT(""));
        }
        return;
    }

    const TSharedRef<FOmnisHttpCompletion> CompletionRef =
        MakeShared<FOmnisHttpCompletion>(MoveTemp(Completion));

    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request =
        FHttpModule::Get().CreateRequest();

    Request->SetURL(BuildApiUrl(Path));
    Request->SetVerb(Verb);
    Request->SetHeader(TEXT("Accept"), TEXT("application/json"));

    if (!JsonBody.IsEmpty())
    {
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Request->SetContentAsString(JsonBody);
    }

    if (!PlayerId.IsEmpty())
    {
        Request->SetHeader(TEXT("X-Omnis-Player-Id"), PlayerId);
    }

    if (!ServiceToken.IsEmpty())
    {
        Request->SetHeader(TEXT("X-Omnis-Service-Token"), ServiceToken);
    }

    Request->SetTimeout(10.0f);

    Request->OnProcessRequestComplete().BindLambda(
        [WeakThis = TWeakObjectPtr<UOmnisOnlineServiceSubsystem>(this), CompletionRef](
            FHttpRequestPtr,
            FHttpResponsePtr Response,
            bool bWasSuccessful)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            const bool bHasResponse = Response.IsValid();
            const int32 StatusCode = bHasResponse ? Response->GetResponseCode() : 0;
            const FString ResponseBody = bHasResponse ? Response->GetContentAsString() : FString();

            if (*CompletionRef)
            {
                (*CompletionRef)(
                    bWasSuccessful &&
                    bHasResponse &&
                    StatusCode >= 200 &&
                    StatusCode < 300,
                    StatusCode,
                    ResponseBody);
            }
        });

    if (!Request->ProcessRequest() && *CompletionRef)
    {
        (*CompletionRef)(false, 0, TEXT(""));
    }
}

FString UOmnisOnlineServiceSubsystem::BuildApiUrl(const FString& Path) const
{
    FString NormalizedPath = Path;
    if (!NormalizedPath.StartsWith(TEXT("/")))
    {
        NormalizedPath = TEXT("/") + NormalizedPath;
    }
    return GatewayBaseUrl + NormalizedPath;
}
