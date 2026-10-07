# Omnis — Mundo Unreal Premium

The canonical map is `/Game/Omnis/Maps/Omnis_OpenWorld`.

The CI materializer creates this map from Unreal Engine 5.6's Open World template, then imports the project's authored source meshes as real native StaticMesh assets and applies a deterministic premium dressing pass.

World Partition, Landscape, Data Layers, One File Per Actor and HLOD are taken from the Open World template rather than simulated in code.

The materialization step is fail-closed: it rejects missing native map output, incomplete native mesh import, missing material assets or insufficient world dressing.

The repository still treats OBJ files as source content. They are not represented as finished photorealistic AAA art.