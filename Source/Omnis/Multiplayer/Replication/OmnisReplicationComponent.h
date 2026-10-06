#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OmnisReplicationComponent.generated.h"

UCLASS(ClassGroup=(Omnis), meta=(BlueprintSpawnableComponent))
class OMNIS_API UOmnisReplicationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOmnisReplicationComponent();

    UFUNCTION(BlueprintCallable, Category="Omnis|Multiplayer")
    void SetAuthoritativeValue(int32 Value);

    UFUNCTION(BlueprintPure, Category="Omnis|Multiplayer")
    int32 GetAuthoritativeValue() const { return AuthoritativeValue; }

protected:
    UFUNCTION(Server, Reliable)
    void ServerSetAuthoritativeValue(int32 Value);

private:
    UPROPERTY(Replicated)
    int32 AuthoritativeValue = 0;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
