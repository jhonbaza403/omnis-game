#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisDialogueAudioSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisDialogueAudioSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Audio")
    void SetVoiceProfile(FName ProfileId);

    UFUNCTION(BlueprintPure, Category="Omnis|Audio")
    FName GetVoiceProfile() const { return VoiceProfile; }

private:
    UPROPERTY()
    FName VoiceProfile;
};
