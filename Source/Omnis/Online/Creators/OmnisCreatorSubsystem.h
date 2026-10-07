#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisCreatorSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisCreation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Creation")
    FString Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Creation")
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Creation")
    FName Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Creation")
    TArray<FName> Elements;

    UPROPERTY(BlueprintReadOnly, Category="Creation")
    int32 Version = 1;

    UPROPERTY(BlueprintReadOnly, Category="Creation")
    int32 Likes = 0;

    UPROPERTY(BlueprintReadOnly, Category="Creation")
    bool bPublished = false;
};

UCLASS()
class OMNIS_API UOmnisCreatorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Creators")
    FString CreateDraft(FString Title, FName Category);

    UFUNCTION(BlueprintCallable, Category="Omnis|Creators")
    bool AddElementToDraft(FString CreationId, FName ElementId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Creators")
    bool PublishCreation(FString CreationId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Creators")
    bool LikeCreation(FString CreationId);

    UFUNCTION(BlueprintPure, Category="Omnis|Creators")
    bool GetCreation(FString CreationId, FOmnisCreation& OutCreation) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Creators")
    TArray<FOmnisCreation> GetPublishedCreations() const;

private:
    UPROPERTY()
    TMap<FString, FOmnisCreation> Creations;
};
