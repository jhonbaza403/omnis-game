#include "Audio/Music/OmnisAudioStateSubsystem.h"

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
