#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisTelemetrySubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisTelemetrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Telemetry")
    bool RecordEvent(FName EventType, const FString& PayloadJson, int32 EventVersion = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Telemetry")
    bool RecordLifecycleEvent(const FString& State);

    UFUNCTION(BlueprintCallable, Category="Omnis|Telemetry")
    bool RecordCrashEvent(const FString& Reason);

    UFUNCTION(BlueprintCallable, Category="Omnis|Telemetry")
    bool RecordPerformanceSample(float FrameTimeMs, float MemoryUsedRatio);

private:
    bool IsSafeEventType(FName EventType) const;
};
