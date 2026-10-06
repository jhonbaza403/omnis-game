#include "Audio/Music/OmnisAudioStateSubsystem.h"

#include "Misc/CoreDelegates.h"

void UOmnisAudioStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    FCoreDelegates::AudioFocusChangedDelegate.AddUObject(this, &ThisClass::HandleAudioFocusChanged);
}

void UOmnisAudioStateSubsystem::Deinitialize()
{
    FCoreDelegates::AudioFocusChangedDelegate.RemoveAll(this);
    Super::Deinitialize();
}

void UOmnisAudioStateSubsystem::SetMasterVolume(float Volume)
{
    MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UOmnisAudioStateSubsystem::SetMusicVolume(float Volume)
{
    MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UOmnisAudioStateSubsystem::SetSFXVolume(float Volume)
{
    SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

float UOmnisAudioStateSubsystem::GetEffectiveMasterVolume() const
{
    return bHasAudioFocus ? MasterVolume : 0.0f;
}

void UOmnisAudioStateSubsystem::HandleAudioFocusChanged(bool bInHasAudioFocus)
{
    bHasAudioFocus = bInHasAudioFocus;
}
