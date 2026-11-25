# OMNIS — World Partition runtime streaming

**Status:** runtime streaming-source integration added to the existing consolidation branch; native UE 5.8 validation still required.

## What the subsystem now does

`UOmnisStreamingSubsystem` no longer treats a region as a name stored in a set. It reads the region/subregion anchor catalog at world-subsystem initialization and creates a transient `UWorldPartitionStreamingSourceComponent` provider at each subregion anchor when `RequestRegion(RegionId)` is called.

Each provider targets `EStreamingSourceTargetState::Activated`. The component is registered with the actor and enabled through the engine API. The subsystem verifies provider registration and asks `UWorldPartitionSubsystem::IsStreamingCompleted(Provider)` whether streaming work associated with that provider has settled.

`ReleaseRegion` disables and destroys the transient provider actors. `Deinitialize` releases any remaining provider actors.

The compatibility method `IsRegionLoaded` delegates to `IsRegionStreamingComplete`. It means that all active provider requests created for that region have completed their affected streaming operations; it does **not** certify that every planned asset or the full geographic region exists.

## Important limits

- The current catalog's `anchor` values are relative Unreal game-space coordinates, not latitude/longitude, GPS, surveyed distances, or DEM samples.
- The subsystem creates one source per catalogued subregion anchor. It loads only the World Partition cells intersecting each source's configured loading range (the engine component's default spherical range when `Shapes` is empty). It does not load an unlimited region polygon.
- This code does not create Landscape, roads, bridges, tunnels, rivers, HLOD, Data Layers, buildings, or runtime cells. Those authored actors/assets must exist in the World Partition map and be marked/configured for spatial loading.
- A map must have World Partition runtime streaming enabled and a valid runtime grid. The player/controller may also provide the normal player streaming source.
- A completion result reports the engine's streaming completion for the providers; a region with no authored spatial actors can complete without making missing content appear.
- Dedicated-server streaming must be configured separately according to the project's server requirements; do not assume the client setting enables it.

## Native validation still required

1. Compile Editor, Game, and Dedicated Server with the exact UE 5.8 installation (UHT/UBT).
2. Run the subsystem in a World Partition test map with spatially loaded test actors near and outside the anchor ranges.
3. Request a region and verify actors in the expected cells activate; verify distant cells remain unloaded.
4. Call `IsRegionStreamingComplete` before and after loading settles and capture the result/log.
5. Release the region and verify the actors unload when no other source (player/camera/vehicle) keeps their cells requested.
6. Repeat with multiple regions, repeated requests, invalid region IDs, and teardown.
7. Capture Unreal Insights/World Partition streaming diagnostics and test client and dedicated-server configurations separately.

## Engine API references

- [World Partition in Unreal Engine 5.8](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)
- [UWorldPartitionStreamingSourceComponent, UE 5.8](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UWorldPartitionStreamingSourceCo-?application_version=5.8)
- [UWorldPartitionSubsystem::IsStreamingCompleted, UE 5.8](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UWorldPartitionSubsystem/IsStreamingCompleted?lang=en-US)
