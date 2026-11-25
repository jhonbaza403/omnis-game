#include "AI/NPC/OmnisNPCDialogueSubsystem.h"
#include "AI/NPC/OmnisNPCCharacter.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/World.h"
#include "HAL/PlatformMisc.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Narrative/Characters/OmnisCharacterStateComponent.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace OmnisNPCDialogue
{
    FString ReadServerSetting(const TCHAR* Name)
    {
        return FPlatformMisc::GetEnvironmentVariable(Name).TrimStartAndEnd();
    }

    FString BuildMemoryContext(const AOmnisNPCCharacter& NPC)
    {
        const UOmnisCharacterStateComponent* State = NPC.CharacterState;
        if (!State) return TEXT("No persistent memory is available.");
        const TArray<FOmnisCharacterMemoryEntry>& Memories = State->GetMemories();
        const int32 FirstMemory = FMath::Max(0, Memories.Num() - 12);
        FString Context;
        for (int32 Index = FirstMemory; Index < Memories.Num(); ++Index)
        {
            const FOmnisCharacterMemoryEntry& Memory = Memories[Index];
            Context += FString::Printf(TEXT("- event=%s; related=%s; importance=%d\n"),
                *Memory.EventId.ToString(), *Memory.RelatedCharacterId.ToString(),
                FMath::Clamp(Memory.Importance, 0, 100));
        }
        return Context.IsEmpty() ? TEXT("No persistent memory is available.") : Context;
    }
}

bool UOmnisNPCDialogueSubsystem::RequestDialogue(
    AOmnisNPCCharacter* NPC, const FString& PlayerMessage, FDialogueCompletion OnComplete)
{
    const auto CompleteImmediately = [&OnComplete](bool bSuccess, const FString& Text)
    {
        if (OnComplete) OnComplete(bSuccess, Text);
    };
    UWorld* World = GetWorld();
    if (!IsValid(NPC) || !NPC->HasAuthority() || !World ||
        World->GetNetMode() == NM_Client || PlayerMessage.TrimStartAndEnd().IsEmpty() ||
        PendingRequests >= MaxPendingRequests)
    {
        CompleteImmediately(false, TEXT("Dialogue service is unavailable."));
        return false;
    }

    const FString Endpoint = OmnisNPCDialogue::ReadServerSetting(TEXT("OMNIS_LLM_ENDPOINT"));
    const FString ApiKey = OmnisNPCDialogue::ReadServerSetting(TEXT("OMNIS_LLM_API_KEY"));
    const FString Model = OmnisNPCDialogue::ReadServerSetting(TEXT("OMNIS_LLM_MODEL"));
    if (!Endpoint.StartsWith(TEXT("https://"), ESearchCase::IgnoreCase) ||
        ApiKey.IsEmpty() || Model.IsEmpty())
    {
        CompleteImmediately(false, TEXT("NPC dialogue provider is not configured on the server."));
        return false;
    }

    const FString SafePlayerMessage = PlayerMessage.TrimStartAndEnd().Left(MaxPlayerMessageCharacters);
    const FString SystemPrompt = FString::Printf(
        TEXT("You write in-character dialogue for an NPC in the fictional Omnis world. ")
        TEXT("Use only supplied character facts and deterministic memory entries. ")
        TEXT("Do not invent remembered events, identity facts, inventory, prices, quest outcomes, ")
        TEXT("faction standing, or world state. If context is missing, say so naturally. ")
        TEXT("Return only a concise dialogue line in the NPC's language. ")
        TEXT("You have no tools and no authority: never encode gameplay actions, commands, ")
        TEXT("function calls, purchases, movement, rewards, or state changes.\n")
        TEXT("NPC id: %s\nName: %s\nBiography: %s\nOccupation: %s\nPersistent memory:\n%s"),
        *NPC->CharacterId.ToString(), *NPC->CharacterDisplayName.ToString(),
        *NPC->CharacterBiography.ToString(), *NPC->OccupationId.ToString(),
        *OmnisNPCDialogue::BuildMemoryContext(*NPC));

    TSharedRef<FJsonObject> SystemMessage = MakeShared<FJsonObject>();
    SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
    SystemMessage->SetStringField(TEXT("content"), SystemPrompt);
    TSharedRef<FJsonObject> UserMessage = MakeShared<FJsonObject>();
    UserMessage->SetStringField(TEXT("role"), TEXT("user"));
    UserMessage->SetStringField(TEXT("content"), SafePlayerMessage);
    TArray<TSharedPtr<FJsonValue>> Messages;
    Messages.Add(MakeShared<FJsonValueObject>(SystemMessage));
    Messages.Add(MakeShared<FJsonValueObject>(UserMessage));

    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("model"), Model);
    Payload->SetArrayField(TEXT("messages"), Messages);
    Payload->SetNumberField(TEXT("temperature"), 0.65);
    Payload->SetNumberField(TEXT("max_tokens"), 180);
    FString PayloadText;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PayloadText);
    if (!FJsonSerializer::Serialize(Payload, Writer))
    {
        CompleteImmediately(false, TEXT("Dialogue request could not be serialized."));
        return false;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Endpoint);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));
    Request->SetTimeout(20.0f);
    Request->SetContentAsString(PayloadText);

    TWeakObjectPtr<UOmnisNPCDialogueSubsystem> WeakThis(this);
    TWeakObjectPtr<AOmnisNPCCharacter> WeakNPC(NPC);
    TSharedRef<FDialogueCompletion> Completion =
        MakeShared<FDialogueCompletion>(MoveTemp(OnComplete));
    ++PendingRequests;
    Request->OnProcessRequestComplete().BindLambda(
        [WeakThis, WeakNPC, Completion](
            FHttpRequestPtr CompletedRequest, FHttpResponsePtr Response, bool bTransportSucceeded) mutable
        {
            if (UOmnisNPCDialogueSubsystem* Self = WeakThis.Get())
                Self->PendingRequests = FMath::Max(0, Self->PendingRequests - 1);

            FString Dialogue;
            bool bSucceeded = false;
            if (bTransportSucceeded && Response.IsValid() &&
                EHttpResponseCodes::IsOk(Response->GetResponseCode()) &&
                Response->GetContentLength() <= 16384)
            {
                TSharedPtr<FJsonObject> Root;
                const TSharedRef<TJsonReader<>> Reader =
                    TJsonReaderFactory<>::Create(Response->GetContentAsString());
                if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>>* Choices = nullptr;
                    if (Root->TryGetArrayField(TEXT("choices"), Choices) && Choices && Choices->Num() > 0 &&
                        (*Choices)[0].IsValid() && (*Choices)[0]->Type == EJson::Object)
                    {
                        const TSharedPtr<FJsonObject> Choice = (*Choices)[0]->AsObject();
                        const TSharedPtr<FJsonObject>* Message = nullptr;
                        if (Choice.IsValid() && Choice->TryGetObjectField(TEXT("message"), Message) &&
                            Message && Message->IsValid() &&
                            (*Message)->TryGetStringField(TEXT("content"), Dialogue))
                        {
                            Dialogue = Dialogue.TrimStartAndEnd().Left(MaxResponseCharacters);
                            bSucceeded = !Dialogue.IsEmpty();
                        }
                    }
                }
            }
            if (bSucceeded)
            {
                if (AOmnisNPCCharacter* NPCActor = WeakNPC.Get())
                {
                    if (NPCActor->HasAuthority() && NPCActor->CharacterState)
                        NPCActor->CharacterState->RecordMemory(TEXT("NPC.Dialogue.Generated"), TEXT("Player"), 3);
                }
            }
            if (*Completion)
                (*Completion)(bSucceeded, bSucceeded ? Dialogue :
                    TEXT("The character has nothing more to say right now."));
        });

    if (!Request->ProcessRequest())
    {
        PendingRequests = FMath::Max(0, PendingRequests - 1);
        if (*Completion)
            (*Completion)(false, TEXT("Dialogue request could not be started."));
        return false;
    }
    return true;
}
