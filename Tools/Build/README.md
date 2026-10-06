# Unreal Engine 5.6 Build

El proyecto requiere una instalación real de Unreal Engine 5.6 para la compilación UBT/UHT.

El runner compartido de GitLab no tiene Unreal Engine 5.6. La compilación debe ejecutarse en una máquina local o runner dedicado con el motor instalado.

Use `Tools/Build/compile_omnis.ps1` en Windows. El flujo usa RunUAT/UnrealBuildTool y no genera una compilación ficticia.
