#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmnisReplicatedWorldStateActor.generated.h"

UCLASS()
class OMNIS_API AOmnisReplicatedWorldStateActor : public AActor
{
    GENERATED_BODY()

public:
    AOmnisReplicatedWorldStateActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    int32 GetCycleIndex() const { return CycleIndex; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    int32 GetWorldRevision() const { return WorldRevision; }

    UFUNCTION(BlueprintPure, Category="Omnis|World")
    FString GetEraName() const { return EraName; }

    void RefreshFromBackend();

private:
    void ApplyBackendResponse(const FString& ResponseBody);

    UPROPERTY(Replicated)
    int32 CycleIndex = 0;

    UPROPERTY(Replicated)
    int32 WorldRevision = 0;

    UPROPERTY(Replicated)
    FString EraName;
};
