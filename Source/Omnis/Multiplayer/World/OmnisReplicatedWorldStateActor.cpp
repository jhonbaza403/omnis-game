#include "Multiplayer/World/OmnisReplicatedWorldStateActor.h"

#include "Dom/JsonObject.h"
#include "Net/UnrealNetwork.h"
#include "Online/Services/OmnisOnlineServiceSubsystem.h"
#include "Serialization/JsonSerializer.h"

AOmnisReplicatedWorldStateActor::AOmnisReplicatedWorldStateActor()
{
    bReplicates = true;
    SetReplicateMovement(false);
    PrimaryActorTick.bCanEverTick = false;
}

void AOmnisReplicatedWorldStateActor::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        RefreshFromBackend();
    }
}

void AOmnisReplicatedWorldStateActor::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, CycleIndex);
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, WorldRevision);
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, EraName);
}

void AOmnisReplicatedWorldStateActor::RefreshFromBackend()
{
    if (!HasAuthority())
    {
        return;
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return;
    }

    UOmnisOnlineServiceSubsystem* Online =
        GameInstance->GetSubsystem<UOmnisOnlineServiceSubsystem>();

    if (!Online)
    {
        return;
    }

    Online->GetWorldCycle(
        [WeakThis = TWeakObjectPtr<AOmnisReplicatedWorldStateActor>(this)](
            bool bSucceeded,
            int32,
            const FString& ResponseBody)
        {
            if (!WeakThis.IsValid() || !bSucceeded)
            {
                return;
            }

            WeakThis->ApplyBackendResponse(ResponseBody);
        });
}

void AOmnisReplicatedWorldStateActor::ApplyBackendResponse(const FString& ResponseBody)
{
    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        return;
    }

    Root->TryGetNumberField(TEXT("cycle_index"), CycleIndex);
    Root->TryGetNumberField(TEXT("world_revision"), WorldRevision);
    Root->TryGetStringField(TEXT("era"), EraName);

    ForceNetUpdate();
}
