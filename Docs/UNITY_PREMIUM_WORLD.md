# Omnis — Premium World Migration

## Estado

La rama Unity 6.3 contiene una vertical slice visual premium generada con geometría determinista en runtime.

### Materializado ahora
- Distrito urbano con edificios de alturas variables.
- Red vial y señalización luminosa.
- Plaza central Nexus.
- Anillos, pilares y balizas del Nexus.
- Cuatro obeliscos del Ciclo.
- Vegetación, rocas y zona de agua.
- Iluminación ambiental, niebla y materiales emisivos.
- Estructura separada para Architecture, Nature y Landmarks.

### Assets Unreal: regla de transferencia

No se convierten ni se renombran falsamente archivos `.uasset`/`.umap`.

La auditoría de `Content/Omnis/SourceAssets` encontró archivos con extensiones transferibles como `.obj` y `.png`, pero su contenido actual es un **puntero Git LFS**, no el binario real. Por tanto, todavía no se copian a `Assets/` como si fueran modelos/texturas utilizables.

Ejemplo auditado:
- `Content/Omnis/SourceAssets/Architecture/Omnis_House.obj`
- `Content/Omnis/SourceAssets/Textures/Omnis_Nexus_Albedo.png`

Ambos contienen metadatos de Git LFS en lugar del asset binario.

## Próxima integración real

Cuando los objetos LFS estén materializados en GitLab, el flujo será:

1. Obtener el binario original.
2. Verificar extensión y tamaño.
3. Copiarlo a `Assets/Art/ImportedFromUnreal/...`.
4. Importarlo en Unity.
5. Crear materiales/prefabs Unity.
6. Sustituir progresivamente geometría procedural equivalente.
7. Validar escena y Play Mode en Unity Editor.

La geometría procedural actual es intencional: mantiene el juego ejecutable y visualmente consistente mientras los binarios reales se incorporan, sin inventar conversiones.
