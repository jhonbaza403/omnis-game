# Omnis — mapa de consolidación técnica

**Rama candidata:** `audit/omnis-360-baseline`  
**Revisión existente:** PR #1 (Draft), con destino `main`.  
**Regla de publicación:** `main` permanece intacta hasta aprobar todas las pruebas nativas.

## Una sola autoridad de gameplay

- **Motor principal:** Unreal Engine 5.8, declarado en `Omnis.uproject`.
- **Módulo de juego:** `Source/Omnis`, único módulo Runtime del proyecto. Las implementaciones existentes de Core, Gameplay, combate, Nexo, narrativa, IA, mundo, online, HUD y persistencia se mantienen aquí bajo sus carpetas de dominio.
- **Plugins:** `OmnisCore`, `OmnisGameplay`, `OmnisNarrative`, `OmnisWorld`, `OmnisAI` y `OmnisOnline` son módulos de extensión habilitados. En el estado inspeccionado sus fuentes son puntos de arranque; no deben recibir copias de los sistemas que ya existen en `Source/Omnis`. Una extracción futura exige transferir propiedad de archivos y dependencias en un único cambio probado.
- **Unity 6.3:** `UnityProject/` se conserva como proyecto secundario e histórico. No es una segunda implementación maestra ni debe recibir nuevas versiones paralelas de los sistemas jugables.
- **Contenido:** las fuentes OBJ/MTL se conservan. UAsset/UMAP son artefactos nativos de Unreal y solo se producen con UE 5.8 real; no se fabrican ni se simulan.

## Propiedad funcional existente

| Dominio | Ubicación canónica |
|---|---|
| Ciclo, eventos, identidad, avatar, Nexo y progresión | `Source/Omnis/Core` |
| Personaje, habilidades, combate, inventario, interacción, construcción, traversal y vehículos | `Source/Omnis/Gameplay` y `Source/Omnis/Systems` |
| IA, NPC, percepción y navegación | `Source/Omnis/AI` |
| Decisiones, misiones, facciones, consecuencias y cinemáticas | `Source/Omnis/Narrative` |
| Mundo, entorno, clima, recursos, streaming y World Partition | `Source/Omnis/World` |
| Sesiones, cuentas, grupos, ranking, creadores y seguridad social | `Source/Omnis/Online` y `Source/Omnis/Multiplayer` |
| HUD, menús, accesibilidad e interfaces de Nexo/inventario | `Source/Omnis/UI` |

Esta tabla expresa la ubicación de código observada, no certifica que todas las funciones estén completas o conectadas en runtime.

## Catálogos de mundo incorporados

Se seleccionaron de `feature/elemental-regions-world-v1` ocho recursos sin duplicar código de gameplay: tres JSON de diseño, tres validadores Node.js y dos documentos técnicos. Se corrigió el ID de la región del vertical slice a `life` para coincidir con el catálogo y el validador ahora verifica el campo real de resolución 3840×2160.

Los datos describen 12 regiones y 1.608 subregiones planificadas; el vertical slice Valle del Despertar permanece marcado como **diseño no verificado como jugable**. Estos catálogos no crean mapas, jefes, assets ni regiones dentro del motor. El CI estático ejecuta sus validadores de estructura y referencias.

## Un único flujo de build por sistema operativo

- Linux: `Tools/Build/build_omnis.sh`.
- Windows: `Tools/Build/build_omnis_windows.ps1`.
- `Tools/Build/compile_omnis.ps1` es solo una interfaz de compatibilidad que delega al builder Windows canónico.
- La creación del mundo comparte las mismas reglas de protección de mapa y assets, tanto desde CI como desde Python.

## Tratamiento de ramas

La rama de auditoría existente es la única candidata a consolidación y reutiliza PR #1. No se crean ramas o PR adicionales para volver a implementar los mismos sistemas.

- `feature/elemental-regions-world-v1`: se incorporaron selectivamente los ocho archivos de datos/documentación/validación mencionados arriba; no se importó su cambio de pipeline GitLab.
- `feature/omnis-complete-foundation`: se conserva sin merge masivo. Está divergida respecto a esta rama y modifica archivos centrales compartidos de personaje, sesión, autenticación y matchmaking; también añade subsistemas de guardado, persistencia cloud, telemetría, plataforma, logros, localización y rendimiento. Estas aportaciones son trabajo recuperable, pero requieren revisión por grupo funcional, análisis de dependencias y compilación/Automation antes de transferirlas.
- Las ramas Unity se conservan como referencia del proyecto secundario; no se importan implementaciones paralelas de gameplay en el motor principal.
- Las ramas antiguas de Cloudflare y prototipos se conservan como referencias, no como infraestructura activa ni como línea de build de Omnis.

Las ramas no se borran ni se sobrescriben. Cualquier aporte adicional se seleccionará por archivo, se asignará a un único propietario funcional y se cubrirá con pruebas antes de integrarlo.

## Puertas obligatorias antes de promover

1. Validación estática, pruebas Python y validadores de catálogos en el SHA candidato.
2. Preflight del motor real UE 5.8.
3. UHT/UBT de Editor, Game y Dedicated Server.
4. Mapa nativo existente preservado o mundo creado solo cuando no exista; validación de 63 mallas nativas y 10 materiales.
5. Unreal Automation Tests ejecutadas y reportadas.
6. Empaquetado real del cliente y del servidor Linux.
7. Smoke test que demuestre el arranque de ambos procesos.
8. Artefactos y logs ligados al mismo SHA de la rama candidata.

Una CI estática exitosa no reemplaza los pasos 2–8. Hasta obtenerlos, el estado correcto es **consolidación de código y controles en revisión; certificación nativa pendiente**.
