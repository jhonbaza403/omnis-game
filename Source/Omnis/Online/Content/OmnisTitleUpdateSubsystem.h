#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisTitleUpdateSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisTitleUpdateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Content")
    bool EnumerateUpdateFiles();

    UFUNCTION(BlueprintCallable, Category="Omnis|Content")
    bool ReadUpdateFile(const FString& FileName);

    UFUNCTION(BlueprintCallable, Category="Omnis|Content")
    bool GetUpdateFileContents(const FString& FileName, TArray<uint8>& OutContents) const;
};
