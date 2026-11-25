# Flujo canónico de compilación — Unreal Engine 5.8

Unreal Engine 5.8.x es el motor principal. Unity 6.3 se conserva en `UnityProject/` como proyecto secundario de referencia; no se deben duplicar allí las nuevas implementaciones de gameplay.

## Entradas canónicas

- Linux: `Tools/Build/build_omnis.sh`. Valida UE 5.8 y construye UHT, Editor, Game y Dedicated Server mediante UBT.
- Windows: `Tools/Build/build_omnis_windows.ps1`. Es la única implementación Windows de compilación; construye Editor y Game y añade Server con `-IncludeServer`.
- Compatibilidad: `Tools/Build/compile_omnis.ps1` conserva el nombre y los parámetros históricos, pero delega todo en `build_omnis_windows.ps1`. No contiene una segunda implementación UHT/UBT.

UBT ejecuta UHT dentro del flujo normal de compilación. No se mantiene una secuencia Windows paralela que vuelva a invocar el mismo proceso.

## Materialización protegida

El mapa `Content/Omnis/Maps/Omnis_OpenWorld.umap` se crea solo cuando no existe. Las dos entradas Python comprueban la ausencia del mapa antes de importar o crear contenido. Las mallas nativas existentes se validan y conservan; los OBJ se importan con `replace_existing=False`.

No se reemplazan automáticamente mapas ni assets revisados. Una migración de un mapa existente requiere un procedimiento separado, respaldado y revisado.

## Certificación nativa

El workflow manual `.github/workflows/omnis-ue58-native-build.yml` requiere un runner Linux autohospedado con etiquetas `self-hosted, linux, x64, omnis-ue58`, Unreal Engine 5.8.x y `UE_ENGINE_ROOT`.

Una ejecución completa debe superar, en el mismo SHA:

1. preflight y versión del motor;
2. UHT/UBT para Editor, Game y Dedicated Server;
3. validación o materialización protegida del mapa y assets nativos;
4. Unreal Automation Tests;
5. empaquetado del cliente Linux y del servidor dedicado;
6. smoke test de arranque del cliente y del servidor;
7. conservación de logs, reportes y paquetes como artefactos.

Los directorios de paquetes son únicos por ejecución. Los scripts se niegan a sobrescribir una carpeta de salida existente.

## Alcance de los resultados

El workflow de validación estática no instala Unreal Engine y no sustituye el build nativo. Un resultado estático verde no certifica C++, UHT/UBT, importación de assets, Automation, empaquetado ni lanzamiento. No promover a `main` hasta obtener evidencia completa del workflow nativo en el commit candidato exacto.
