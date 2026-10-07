#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisPartySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisPartyMember
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Party")
    FName UserId;

    UPROPERTY(BlueprintReadOnly, Category="Party")
    FString DisplayName;

    UPROPERTY(BlueprintReadOnly, Category="Party")
    bool bReady = false;
};

UCLASS()
class OMNIS_API UOmnisPartySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Social")
    bool CreateParty(FName OwnerId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Social")
    bool AddMember(FName UserId, FString DisplayName);

    UFUNCTION(BlueprintCallable, Category="Omnis|Social")
    bool RemoveMember(FName UserId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Social")
    bool SetMemberReady(FName UserId, bool bReady);

    UFUNCTION(BlueprintPure, Category="Omnis|Social")
    TArray<FOmnisPartyMember> GetMembers() const { return Members; }

    UFUNCTION(BlueprintPure, Category="Omnis|Social")
    FName GetPartyId() const { return PartyId; }

private:
    bool IsBlockedFromParty(FName UserId) const;

    UPROPERTY()
    FName PartyId;

    UPROPERTY()
    TArray<FOmnisPartyMember> Members;
};
