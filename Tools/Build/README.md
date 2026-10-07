# Unreal Engine 5.8 — build real

El proyecto debe compilarse contra una instalación real de **Unreal Engine 5.8.x**. El runner compartido que ejecuta validaciones Python no contiene Unreal Engine, por lo que no debe usarse para la prueba C++/UHT/UBT.

## Pipeline de GitLab

El job `unreal_build` usa exclusivamente el tag:
`unreal58-linux`

La máquina del runner debe tener:

- Unreal Engine 5.8.x instalado de forma válida.
- `UE_ENGINE_ROOT` definido en el entorno del runner, apuntando a la raíz que contiene `Engine/`.
- `RunUBT.sh` ejecutable en `$UE_ENGINE_ROOT/Engine/Build/BatchFiles/`.
- Toolchain C++ de Linux compatible con UE 5.8.

El job es **manual** y depende de `python_tests`. Si no existe un runner con el tag `unreal58-linux`, el job debe quedar pendiente y no ejecutarse en el runner genérico.

## Secuencia de build

1. Comprueba que el motor sea 5.8.x.
2. Ejecuta **UnrealHeaderTool** mediante el modo UHT de UBT para `OmnisEditor`.
3. Verifica que UHT haya generado código de reflexión.
4. Ejecuta **UBT + compilación + enlace** del target `OmnisEditor Linux Development`.
5. Ejecuta **UBT + compilación + enlace** del target `Omnis Linux Development`.
6. Comprueba que existan binarios/módulos de Omnis bajo `Binaries/`.
7. Conserva `Saved/Logs/omnis_ue56_build.log` como artefacto del job.

Epic documenta que UHT forma parte del flujo de compilación de Unreal y que UBT invoca UHT antes de compilar el código generado; los targets de proyecto se definen mediante `.Target.cs` y los módulos mediante `.Build.cs`.
https://dev.epicgames.com/documentation/unreal-engine/unreal-header-tool-for-unreal-engine
https://dev.epicgames.com/documentation/en-us/unreal-engine/compiling-game-projects-in-unreal-engine-using-cplusplus

## Ejecución local Windows

Use:
`Tools/Build/compile_omnis.ps1`

Ejemplo:
`.\Tools\Build\compile_omnis.ps1 -UnrealRoot "C:\Program Files\Epic Games\UE_5.8"`

El script ejecuta UHT, luego UBT para Editor y Game. No declara una compilación exitosa hasta que los comandos reales de Unreal terminan con código 0.

## Importante

No se deben crear mocks, migraciones ficticias, binarios falsos ni una imagen de contenedor que pretenda contener UE 5.8 sin una instalación válida del motor. Para un build real de proyecto C++, la máquina ejecutora debe disponer del motor y del toolchain correspondiente.