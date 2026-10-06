#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisFriendsSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisFriendsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    bool AddFriend(FName UserId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Online")
    void RemoveFriend(FName UserId);

    UFUNCTION(BlueprintPure, Category="Omnis|Online")
    bool IsFriend(FName UserId) const;

private:
    UPROPERTY()
    TSet<FName> Friends;
};
