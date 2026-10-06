#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisDialogueSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnOmnisDialogueChanged, FName, SpeakerId, FText, Line, bool, bFinished);

UCLASS()
class OMNIS_API UOmnisDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void StartDialogue(FName SpeakerId, const TArray<FText>& Lines);

    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void AdvanceDialogue();

    UFUNCTION(BlueprintCallable, Category="Omnis|Narrative")
    void EndDialogue();

    UFUNCTION(BlueprintPure, Category="Omnis|Narrative")
    bool IsDialogueActive() const { return bActive; }

    UPROPERTY(BlueprintAssignable, Category="Omnis|Narrative")
    FOnOmnisDialogueChanged OnDialogueChanged;

private:
    UPROPERTY()
    FName CurrentSpeaker;

    UPROPERTY()
    TArray<FText> CurrentLines;

    int32 CurrentIndex = INDEX_NONE;
    bool bActive = false;
};
