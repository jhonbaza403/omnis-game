#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OmnisWorldPartitionSubsystem.generated.h"

UCLASS()
class OMNIS_API UOmnisWorldPartitionSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Omnis|WorldPartition")
    FIntVector GetCellForLocation(FVector WorldLocation, int32 CellSize = 10000) const;
};
