# ImportedFromUnreal

Directorio reservado para assets binarios reales recuperados de Git LFS o exportados desde Unreal.

No colocar aquí:
- `.uasset` o `.umap` esperando que Unity los importe directamente.
- punteros de Git LFS (`version https://git-lfs.github.com/spec/v1`).
- archivos renombrados para aparentar una conversión.

Formatos previstos para importación Unity:
- `.fbx`
- `.obj`
- `.gltf`
- `.glb`
- `.png`
- `.jpg`
- `.tga`
- `.wav`
- `.ogg`

Cada asset debe conservar trazabilidad hacia su ruta original en `Content/Omnis/SourceAssets`.
