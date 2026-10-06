#include "Diagnostics/OmnisTelemetrySubsystem.h"

#include "Dom/JsonObject.h"
#include "Online/Persistence/OmnisCloudPersistenceSubsystem.h"
#include "Serialization/JsonSerializer.h"

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

bool UOmnisTelemetrySubsystem::RecordEvent(FName EventType, const FString& PayloadJson, int32 EventVersion)
{
    if (!IsSafeEventType(EventType) || EventVersion < 1)
    {
        return false;
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UOmnisCloudPersistenceSubsystem* Cloud = GameInstance->GetSubsystem<UOmnisCloudPersistenceSubsystem>())
        {
            return Cloud->SubmitEvent(EventType, PayloadJson, EventVersion);
        }
    }

    return false;
}

bool UOmnisTelemetrySubsystem::RecordLifecycleEvent(const FString& State)
{
    const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("state"), State.Left(64));
    return RecordEvent(TEXT("client_lifecycle"), SerializeObject(Payload));
}

bool UOmnisTelemetrySubsystem::RecordCrashEvent(const FString& Reason)
{
    const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("reason"), Reason.Left(120));
    return RecordEvent(TEXT("client_crash"), SerializeObject(Payload));
}

bool UOmnisTelemetrySubsystem::RecordPerformanceSample(float FrameTimeMs, float MemoryUsedRatio)
{
    const TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetNumberField(TEXT("frame_time_ms"), FMath::Max(0.0f, FrameTimeMs));
    Payload->SetNumberField(TEXT("memory_used_ratio"), FMath::Clamp(MemoryUsedRatio, 0.0f, 1.0f));
    return RecordEvent(TEXT("performance_sample"), SerializeObject(Payload));
}

bool UOmnisTelemetrySubsystem::IsSafeEventType(FName EventType) const
{
    const FString Value = EventType.ToString();
    if (Value.IsEmpty() || Value.Len() > 120)
    {
        return false;
    }

    for (const TCHAR Character : Value)
    {
        if (!(FChar::IsAlnum(Character) || Character == TCHAR('_') || Character == TCHAR('-')))
        {
            return false;
        }
    }

    return true;
}
