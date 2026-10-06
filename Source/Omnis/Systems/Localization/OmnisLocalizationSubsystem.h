#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisLocalizationSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisLocalizationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Localization")
    bool SetCulture(const FString& Culture);

    UFUNCTION(BlueprintPure, Category="Omnis|Localization")
    FString GetCurrentCulture() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Localization")
    TArray<FString> GetSupportedCultures() const;

    UFUNCTION(BlueprintPure, Category="Omnis|Localization")
    FString GetNativeCulture() const;

private:
    TArray<FString> LoadSupportedCultures() const;
};
