#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAudioStateSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisAudioStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category="Omnis|Audio")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category="Omnis|Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintPure, Category="Omnis|Audio")
    float GetMasterVolume() const { return MasterVolume; }

    UFUNCTION(BlueprintPure, Category="Omnis|Audio")
    float GetMusicVolume() const { return MusicVolume; }

    UFUNCTION(BlueprintPure, Category="Omnis|Audio")
    float GetSFXVolume() const { return SFXVolume; }

private:
    UPROPERTY()
    float MasterVolume = 1.0f;

    UPROPERTY()
    float MusicVolume = 1.0f;

    UPROPERTY()
    float SFXVolume = 1.0f;
};
