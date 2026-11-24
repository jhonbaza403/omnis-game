#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "OmnisCharacterDefinition.generated.h"

class UStaticMesh;

UCLASS(BlueprintType)
class OMNIS_API UOmnisCharacterDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Identity")
    FName CharacterId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Identity")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Identity", meta=(MultiLine=true))
    FText Biography;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Identity")
    FName OccupationId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Identity")
    FName HomeLocationId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Identity")
    FName WorkLocationId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Visual")
    TSoftObjectPtr<UStaticMesh> VisualMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="1.0", ClampMax="20.0"))
    float LifeUpdateInterval = 3.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float WorkStartHour = 7.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float WorkEndHour = 18.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float RestStartHour = 21.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="24.0"))
    float RestEndHour = 6.0f;

    // Bit 0 = Monday, bit 6 = Sunday. 0x7F means every day.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0", ClampMax="127"))
    int32 WorkDaysMask = 0x7F;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="100.0"))
    float HungerThreshold = 65.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="100.0"))
    float EnergyThreshold = 25.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="0.0", ClampMax="100.0"))
    float SocialThreshold = 75.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="100.0", ClampMax="5000.0"))
    float ExploreRadius = 1800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Life", meta=(ClampMin="100.0", ClampMax="5000.0"))
    float SocialRadius = 1800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Movement", meta=(ClampMin="50.0", ClampMax="800.0"))
    float WalkSpeed = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Movement")
    FVector WorkLocationOffset = FVector(1200.0f, 0.0f, 0.0f);
};
