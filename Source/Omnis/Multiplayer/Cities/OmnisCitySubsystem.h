#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisCitySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisCity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CityId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Population = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName FactionId;
};

UCLASS()
class OMNIS_API UOmnisCitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Cities")
    void RegisterCity(const FOmnisCity& City);

    UFUNCTION(BlueprintPure, Category="Omnis|Cities")
    FOmnisCity GetCity(FName CityId) const;

private:
    UPROPERTY()
    TMap<FName, FOmnisCity> Cities;
};
