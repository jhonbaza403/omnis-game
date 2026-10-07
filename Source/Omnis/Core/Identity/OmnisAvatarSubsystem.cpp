#include "Core/Identity/OmnisAvatarSubsystem.h"

void UOmnisAvatarSubsystem::InitializeStarterProfile()
{
    if (Profile.UnlockedCosmetics.Num() == 0)
    {
        Profile.UnlockedCosmetics.Add(TEXT("Wanderer.Default"));
        Profile.UnlockedCosmetics.Add(TEXT("Hair.Default"));
        Profile.UnlockedCosmetics.Add(TEXT("Emote.Starter"));
    }

    Profile.AvatarId = TEXT("Wanderer");
    if (Profile.OutfitId.IsNone())
    {
        Profile.OutfitId = TEXT("Wanderer.Default");
    }
    if (Profile.HairId.IsNone())
    {
        Profile.HairId = TEXT("Hair.Default");
    }
    if (Profile.EmoteSetId.IsNone())
    {
        Profile.EmoteSetId = TEXT("Emote.Starter");
    }
}

bool UOmnisAvatarSubsystem::UnlockCosmetic(FName CosmeticId)
{
    if (CosmeticId.IsNone())
    {
        return false;
    }

    return Profile.UnlockedCosmetics.Add(CosmeticId) > 0;
}

bool UOmnisAvatarSubsystem::EquipOutfit(FName OutfitId)
{
    if (OutfitId.IsNone() || !HasCosmetic(OutfitId))
    {
        return false;
    }

    Profile.OutfitId = OutfitId;
    return true;
}

bool UOmnisAvatarSubsystem::EquipPet(FName PetId)
{
    if (PetId.IsNone() || !HasCosmetic(PetId))
    {
        return false;
    }

    Profile.PetId = PetId;
    return true;
}

bool UOmnisAvatarSubsystem::HasCosmetic(FName CosmeticId) const
{
    return !CosmeticId.IsNone() && Profile.UnlockedCosmetics.Contains(CosmeticId);
}
