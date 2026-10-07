#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisSafetySubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisSafetyReason : uint8
{
    Harassment,
    Spam,
    InappropriateContent,
    Cheating,
    Other
};

USTRUCT(BlueprintType)
struct FOmnisSafetyReport
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Safety")
    FName ReporterId;

    UPROPERTY(BlueprintReadOnly, Category="Safety")
    FName TargetId;

    UPROPERTY(BlueprintReadOnly, Category="Safety")
    EOmnisSafetyReason Reason = EOmnisSafetyReason::Other;

    UPROPERTY(BlueprintReadOnly, Category="Safety")
    FString Context;
};

UCLASS()
class OMNIS_API UOmnisSafetySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Safety")
    bool BlockPlayer(FName UserId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Safety")
    bool UnblockPlayer(FName UserId);

    UFUNCTION(BlueprintCallable, Category="Omnis|Safety")
    bool ReportPlayer(FName ReporterId, FName TargetId, EOmnisSafetyReason Reason, FString Context);

    UFUNCTION(BlueprintPure, Category="Omnis|Safety")
    bool IsBlocked(FName UserId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Safety")
    bool CanInteract(FName UserId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Safety")
    TArray<FOmnisSafetyReport> GetRecentReports() const { return Reports; }

private:
    UPROPERTY()
    TSet<FName> BlockedPlayers;

    UPROPERTY()
    TArray<FOmnisSafetyReport> Reports;
};
