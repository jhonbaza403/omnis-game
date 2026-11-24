#include "Multiplayer/World/OmnisReplicatedWorldStateActor.h"

#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Dom/JsonObject.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Narrative/Consequences/OmnisConsequenceSubsystem.h"
#include "Narrative/Factions/OmnisFactionSubsystem.h"
#include "Narrative/Quests/OmnisQuestSubsystem.h"
#include "Narrative/Reputation/OmnisReputationSubsystem.h"
#include "Online/Services/OmnisOnlineServiceSubsystem.h"
#include "Serialization/JsonSerializer.h"
#include "World/WorldState/OmnisWorldStateSubsystem.h"

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
        RefreshNarrativeState();
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
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, QuestStates);
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, ActiveConsequences);
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, ReputationValues);
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, FactionStandings);
    DOREPLIFETIME(AOmnisReplicatedWorldStateActor, WorldValues);
}

void AOmnisReplicatedWorldStateActor::RefreshNarrativeState()
{
    if (!HasAuthority())
    {
        return;
    }

    UGameInstance* GameInstance = GetGameInstance();
    UWorld* World = GetWorld();
    if (!GameInstance || !World)
    {
        return;
    }

    QuestStates.Reset();
    ActiveConsequences.Reset();
    ReputationValues.Reset();
    FactionStandings.Reset();
    WorldValues.Reset();

    if (UOmnisQuestSubsystem* Quests = GameInstance->GetSubsystem<UOmnisQuestSubsystem>())
    {
        for (const FOmnisQuestProgressSnapshot& Progress : Quests->ExportProgress())
        {
            FOmnisReplicatedQuestState State;
            State.QuestId = Progress.QuestId;
            State.State = Progress.State;
            State.Attempts = Progress.Attempts;
            State.RecoveryQuestId = Progress.RecoveryQuestId;
            State.FailureConsequenceId = Progress.FailureConsequenceId;
            QuestStates.Add(State);
        }
    }

    if (UOmnisConsequenceSubsystem* Consequences =
        GameInstance->GetSubsystem<UOmnisConsequenceSubsystem>())
    {
        ActiveConsequences = Consequences->ExportConsequences();
    }

    if (UOmnisReputationSubsystem* Reputation =
        GameInstance->GetSubsystem<UOmnisReputationSubsystem>())
    {
        TMap<FName, int32> Values = Reputation->ExportReputation();
        for (const TPair<FName, int32>& Pair : Values)
        {
            FOmnisReplicatedWorldValue Value;
            Value.Key = Pair.Key;
            Value.Value = Pair.Value;
            ReputationValues.Add(Value);
        }
    }

    if (UOmnisFactionSubsystem* Factions =
        GameInstance->GetSubsystem<UOmnisFactionSubsystem>())
    {
        TMap<FName, int32> Values = Factions->ExportStandings();
        for (const TPair<FName, int32>& Pair : Values)
        {
            FOmnisReplicatedWorldValue Value;
            Value.Key = Pair.Key;
            Value.Value = Pair.Value;
            FactionStandings.Add(Value);
        }
    }

    if (UOmnisWorldStateSubsystem* WorldState = World->GetSubsystem<UOmnisWorldStateSubsystem>())
    {
        TMap<FName, int32> Values = WorldState->ExportStates();
        for (const TPair<FName, int32>& Pair : Values)
        {
            FOmnisReplicatedWorldValue Value;
            Value.Key = Pair.Key;
            Value.Value = Pair.Value;
            WorldValues.Add(Value);
        }
    }

    auto SortValues = [](TArray<FOmnisReplicatedWorldValue>& Values)
    {
        Values.Sort([](const FOmnisReplicatedWorldValue& A, const FOmnisReplicatedWorldValue& B)
        {
            return A.Key.Compare(B.Key) < 0;
        });
    };
    SortValues(ReputationValues);
    SortValues(FactionStandings);
    SortValues(WorldValues);

    ForceNetUpdate();
    OnNarrativeStateChanged.Broadcast();
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

void AOmnisReplicatedWorldStateActor::OnRep_NarrativeState()
{
    OnNarrativeStateChanged.Broadcast();
}
