#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisAvatarSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisAvatarProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avatar")
    FName AvatarId = TEXT("Wanderer");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avatar")
    FName OutfitId = TEXT("Wanderer.Default");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avatar")
    FName HairId = TEXT("Hair.Default");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avatar")
    FName EmoteSetId = TEXT("Emote.Starter");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avatar")
    FName PetId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Avatar")
    TSet<FName> UnlockedCosmetics;
};

UCLASS()
class OMNIS_API UOmnisAvatarSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Avatar")
    void InitializeStarterProfile();

    UFUNCTION(BlueprintCallable, Category="Omnis|Avatar")
    bool UnlockCosmetic(FName CosmeticId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Avatar")
    bool EquipOutfit(FName OutfitId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Avatar")
    bool EquipPet(FName PetId);

    UFUNCTION(BlueprintPure, Category="Omnis|Avatar")
    bool HasCosmetic(FName CosmeticId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Avatar")
    FOmnisAvatarProfile GetProfile() const { return Profile; }

private:
    UPROPERTY()
    FOmnisAvatarProfile Profile;
};
