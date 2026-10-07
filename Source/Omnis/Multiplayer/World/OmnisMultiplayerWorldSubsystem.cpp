#include "Multiplayer/World/OmnisMultiplayerWorldSubsystem.h"

#include "Dom/JsonObject.h"
#include "Online/Services/OmnisOnlineServiceSubsystem.h"
#include "Serialization/JsonSerializer.h"

void UOmnisMultiplayerWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    SetWorldId(TEXT("PrimerCiclo"));
    RefreshWorldState();
}

void UOmnisMultiplayerWorldSubsystem::Deinitialize()
{
    WorldId = NAME_None;
    WorldRevision = 0;
    CycleIndex = 0;
    EraName.Empty();

    Super::Deinitialize();
}

void UOmnisMultiplayerWorldSubsystem::SetWorldId(FName NewWorldId)
{
    if (!NewWorldId.IsNone())
    {
        WorldId = NewWorldId;
    }
}

void UOmnisMultiplayerWorldSubsystem::RefreshWorldState()
{
    UOmnisOnlineServiceSubsystem* Online = GetGameInstance()
        ? GetGameInstance()->GetSubsystem<UOmnisOnlineServiceSubsystem>()
        : nullptr;

    if (!Online)
    {
        return;
    }

    Online->GetWorldCycle(
        [WeakThis = TWeakObjectPtr<UOmnisMultiplayerWorldSubsystem>(this)](
            bool bSucceeded,
            int32,
            const FString& ResponseBody)
        {
            if (!WeakThis.IsValid() || !bSucceeded)
            {
                return;
            }

            TSharedPtr<FJsonObject> Root;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
            if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
            {
                return;
            }

            int32 Revision = 0;
            int32 Cycle = 0;
            FString Era;

            Root->TryGetNumberField(TEXT("world_revision"), Revision);
            Root->TryGetNumberField(TEXT("cycle_index"), Cycle);
            Root->TryGetStringField(TEXT("era"), Era);

            WeakThis->WorldRevision = Revision;
            WeakThis->CycleIndex = Cycle;

            if (!Era.IsEmpty())
            {
                WeakThis->EraName = Era;
            }
        });
}
