#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisDialogueViewSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisDialogueViewSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|UI")
    void SetSpeaker(FName SpeakerId);

    UFUNCTION(BlueprintPure, Category="Omnis|UI")
    FName GetSpeaker() const { return SpeakerId; }

private:
    UPROPERTY()
    FName SpeakerId;
};
