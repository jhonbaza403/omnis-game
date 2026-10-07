#include "Online/Services/OmnisOnlineServiceSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformMisc.h"

void UOmnisOnlineServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    GatewayBaseUrl = FPlatformMisc::GetEnvironmentVariable(TEXT("OMNIS_BACKEND_URL"));
    if (GatewayBaseUrl.IsEmpty() && GConfig)
    {
        GConfig->GetString(
            TEXT("Omnis.Online"),
            TEXT("GatewayUrl"),
            GatewayBaseUrl,
            GGameIni);
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
    SetServiceAvailable(TEXT("gateway"), false);
    RefreshGatewayHealth();
}

void UOmnisOnlineServiceSubsystem::Deinitialize()
{
    Services.Empty();
    GatewayBaseUrl.Empty();
    PlayerId.Empty();

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
            const int32 StatusCode = bHasResponse
                ? Response->GetResponseCode()
                : 0;
            const FString ResponseBody = bHasResponse
                ? Response->GetContentAsString()
                : FString();

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
