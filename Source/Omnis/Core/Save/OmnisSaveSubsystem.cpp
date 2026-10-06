#include "Core/Save/OmnisSaveSubsystem.h"
#include "Core/Cycle/OmnisCycleSubsystem.h"
#include "Core/Nexus/OmnisNexusSubsystem.h"
#include "Core/Progression/OmnisProgressionSubsystem.h"
#include "Core/Save/OmnisSaveGame.h"
#include "Kismet/GameplayStatics.h"

FString UOmnisSaveSubsystem::MakeSlotName(int32 Slot) const
{
    return FString::Printf(TEXT("OmnisSlot_%d"), NormalizeSlot(Slot));
}

int32 UOmnisSaveSubsystem::NormalizeSlot(int32 Slot) const
{
    return FMath::Clamp(Slot, MinSlot, MaxSlot);
}

bool UOmnisSaveSubsystem::SaveToSlot(int32 Slot)
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return false;
    }

    UOmnisSaveGame* SaveGame = Cast<UOmnisSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UOmnisSaveGame::StaticClass())
    );

    if (!SaveGame)
    {
        return false;
    }

    if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
    {
        SaveGame->CycleState = Cycle->GetCycleState();
    }

    if (UOmnisProgressionSubsystem* Progression = GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
    {
        const FOmnisPlayerProgression Current = Progression->GetProgression();
        SaveGame->ProgressionExperience = Current.Experience;
        SaveGame->ProgressionLevel = Current.Level;
    }

    if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
    {
        SaveGame->PermanentUpgradePoints = Nexus->GetPermanentUpgradePoints();
        SaveGame->PermanentVitalityLevel = Nexus->GetPermanentVitalityLevel();
    }

    const int32 SafeSlot = NormalizeSlot(Slot);
    const bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, MakeSlotName(SafeSlot), 0);
    if (bSaved)
    {
        LastUsedSlot = SafeSlot;
    }

    return bSaved;
}

bool UOmnisSaveSubsystem::LoadFromSlot(int32 Slot)
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance || !HasSave(Slot))
    {
        return false;
    }

    const int32 SafeSlot = NormalizeSlot(Slot);
    UOmnisSaveGame* SaveGame = Cast<UOmnisSaveGame>(
        UGameplayStatics::LoadGameFromSlot(MakeSlotName(SafeSlot), 0)
    );

    if (!SaveGame)
    {
        return false;
    }

    if (UOmnisCycleSubsystem* Cycle = GameInstance->GetSubsystem<UOmnisCycleSubsystem>())
    {
        Cycle->RestoreState(SaveGame->CycleState);
    }

    if (UOmnisProgressionSubsystem* Progression = GameInstance->GetSubsystem<UOmnisProgressionSubsystem>())
    {
        Progression->RestoreProgression(SaveGame->ProgressionExperience, SaveGame->ProgressionLevel);
    }

    if (UOmnisNexusSubsystem* Nexus = GameInstance->GetSubsystem<UOmnisNexusSubsystem>())
    {
        Nexus->RestoreProgression(SaveGame->PermanentUpgradePoints, SaveGame->PermanentVitalityLevel);
    }

    LastUsedSlot = SafeSlot;
    return true;
}

bool UOmnisSaveSubsystem::DeleteSlot(int32 Slot)
{
    const int32 SafeSlot = NormalizeSlot(Slot);
    if (!HasSave(SafeSlot))
    {
        return true;
    }

    UGameplayStatics::DeleteGameInSlot(MakeSlotName(SafeSlot), 0);
    return !HasSave(SafeSlot);
}

bool UOmnisSaveSubsystem::HasSave(int32 Slot) const
{
    return UGameplayStatics::DoesSaveGameExist(MakeSlotName(Slot), 0);
}
