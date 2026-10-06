#include "World/WorldPartition/OmnisWorldPartitionSubsystem.h"

FIntVector UOmnisWorldPartitionSubsystem::GetCellForLocation(FVector WorldLocation, int32 CellSize) const
{
    const int32 SafeCellSize = FMath::Max(1, CellSize);
    return FIntVector(
        FMath::FloorToInt(WorldLocation.X / SafeCellSize),
        FMath::FloorToInt(WorldLocation.Y / SafeCellSize),
        FMath::FloorToInt(WorldLocation.Z / SafeCellSize)
    );
}
