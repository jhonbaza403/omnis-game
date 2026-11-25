#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/Function.h"
#include "OmnisNPCDialogueSubsystem.generated.h"
class AOmnisNPCCharacter;

/** Server-side bridge: deterministic NPC state in, dialogue text out. */
UCLASS()
class OMNIS_API UOmnisNPCDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    using FDialogueCompletion = TFunction<void(bool bSucceeded, const FString& Dialogue)>;
    bool RequestDialogue(AOmnisNPCCharacter* NPC, const FString& PlayerMessage, FDialogueCompletion OnComplete);
    int32 GetPendingRequestCount() const { return PendingRequests; }
private:
    static constexpr int32 MaxPendingRequests = 8;
    static constexpr int32 MaxPlayerMessageCharacters = 1200;
    static constexpr int32 MaxResponseCharacters = 1600;
    int32 PendingRequests = 0;
};
