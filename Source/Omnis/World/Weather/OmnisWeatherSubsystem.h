#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisWeatherSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOmnisWeather : uint8
{
    Clear,
    Cloudy,
    Rain,
    Storm,
    Fog
};

UCLASS()
class OMNIS_API UOmnisWeatherSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Weather")
    void SetWeather(EOmnisWeather NewWeather);

    UFUNCTION(BlueprintPure, Category="Omnis|Weather")
    EOmnisWeather GetWeather() const { return Weather; }

private:
    UPROPERTY()
    EOmnisWeather Weather = EOmnisWeather::Clear;
};
