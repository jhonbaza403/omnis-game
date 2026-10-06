#include "Online/Persistence/OmnisCloudPersistenceSubsystem.h"
#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UOmnisCloudPersistenceSubsystem::SetApiBaseUrl(FString NewBaseUrl)
{
    NewBaseUrl.TrimStartAndEndInline();
    while (NewBaseUrl.EndsWith(TEXT("/")))
    {
        NewBaseUrl.LeftChopInline(1, false);
    }

    ApiBaseUrl = MoveTemp(NewBaseUrl);
}

void UOmnisCloudPersistenceSubsystem::SetAccessToken(FString NewAccessToken)
{
    NewAccessToken.TrimStartAndEndInline();
    AccessToken = MoveTemp(NewAccessToken);
}

bool UOmnisCloudPersistenceSubsystem::IsValidSlot(int32 Slot) const
{
    return Slot >= 1 && Slot <= 10;
}

bool UOmnisCloudPersistenceSubsystem::IsValidJsonObject(const FString& Json) const
{
    const FString Trimmed = Json.TrimStartAndEnd();
    if (Trimmed.IsEmpty())
    {
        return false;
    }

    TSharedPtr<FJsonObject> Object;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Trimmed);
    return FJsonSerializer::Deserialize(Reader, Object) && Object.IsValid();
}

void UOmnisCloudPersistenceSubsystem::ExecuteRequest(FName Operation, const FString& Method, const FString& Path, const FString& Body)
{
    if (ApiBaseUrl.IsEmpty() || AccessToken.IsEmpty())
    {
        OnRequestCompleted.Broadcast(Operation, false, 0, TEXT("cloud persistence is not configured"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(ApiBaseUrl + Path);
    Request->SetVerb(Method);
    Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + AccessToken);

    if (!Body.IsEmpty())
    {
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Request->SetContentAsString(Body);
    }

    TWeakObjectPtr<UOmnisCloudPersistenceSubsystem> WeakThis(this);
    Request->OnProcessRequestComplete().BindLambda(
        [WeakThis, Operation](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            const int32 StatusCode = Response.IsValid() ? Response->GetResponseCode() : 0;
            const FString Payload = Response.IsValid() ? Response->GetContentAsString() : FString();
            const bool bSuccess = bWasSuccessful && Response.IsValid() && StatusCode >= 200 && StatusCode < 300;
            WeakThis->OnRequestCompleted.Broadcast(Operation, bSuccess, StatusCode, Payload);
        }
    );

    if (!Request->ProcessRequest())
    {
        OnRequestCompleted.Broadcast(Operation, false, 0, TEXT("request could not be started"));
    }
}

bool UOmnisCloudPersistenceSubsystem::SaveSlotJson(int32 Slot, const FString& StateJson, int32 SaveVersion)
{
    if (!IsValidSlot(Slot) || SaveVersion < 1 || !IsValidJsonObject(StateJson))
    {
        return false;
    }

    const FString Body = FString::Printf(
        TEXT("{\"save_version\":%d,\"state\":%s}"),
        SaveVersion,
        *StateJson
    );

    ExecuteRequest(
        TEXT("save_slot"),
        TEXT("PUT"),
        FString::Printf(TEXT("/v1/saves/%d"), Slot),
        Body
    );
    return true;
}

bool UOmnisCloudPersistenceSubsystem::LoadSlotJson(int32 Slot)
{
    if (!IsValidSlot(Slot))
    {
        return false;
    }

    ExecuteRequest(
        TEXT("load_slot"),
        TEXT("GET"),
        FString::Printf(TEXT("/v1/saves/%d"), Slot),
        FString()
    );
    return true;
}

bool UOmnisCloudPersistenceSubsystem::DeleteSlot(int32 Slot)
{
    if (!IsValidSlot(Slot))
    {
        return false;
    }

    ExecuteRequest(
        TEXT("delete_slot"),
        TEXT("DELETE"),
        FString::Printf(TEXT("/v1/saves/%d"), Slot),
        FString()
    );
    return true;
}

bool UOmnisCloudPersistenceSubsystem::SubmitEvent(FName EventType, const FString& PayloadJson, int32 EventVersion)
{
    if (EventType.IsNone() || EventVersion < 1 || !IsValidJsonObject(PayloadJson))
    {
        return false;
    }

    const FString EscapedEventType = EventType.ToString()
        .Replace(TEXT("\"), TEXT("\\"))
        .Replace(TEXT("""), TEXT("\""));

    const FString Body = FString::Printf(
        TEXT("{\"event_type\":\"%s\",\"event_version\":%d,\"payload\":%s}"),
        *EscapedEventType,
        EventVersion,
        *PayloadJson
    );

    ExecuteRequest(TEXT("submit_event"), TEXT("POST"), TEXT("/v1/events"), Body);
    return true;
}
