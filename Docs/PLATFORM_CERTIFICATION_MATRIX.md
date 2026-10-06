# Omnis — Matriz de plataformas y certificación

## Objetivo

La misma base de gameplay debe poder cocinarse y empaquetarse para las plataformas objetivo sin duplicar gameplay, guardado, narrativa, accesibilidad ni lógica online.

| Plataforma | Base común | Adaptador de servicios | Estado de certificación |
| --- | --- | --- | --- |
| PC/Windows | UE5.6 + Omnis | OnlineSubsystem / servicios Omnis | Validable con hardware disponible |
| PlayStation 5 | UE5.6 + Omnis | Provider oficial PlayStation | Requiere SDK/hardware/autorización |
| Xbox Series X|S | UE5.6 + Omnis | Provider oficial Xbox | Requiere GDK/hardware/autorización |
| Nintendo Switch 2 | UE5.6 + Omnis | Provider oficial Nintendo | Requiere SDK/hardware/autorización |

## Contratos comunes cerrados

- input y navegación por mando;
- SaveGame local y persistencia cloud;
- identidad y perfiles;
- achievements mediante IOnlineAchievements;
- entitlements mediante IOnlineEntitlements;
- title-file/update metadata mediante IOnlineTitleFile;
- lifecycle suspend/resume;
- HDR, resolución dinámica y escalabilidad mediante UGameUserSettings;
- métricas de memoria mediante FPlatformMemory;
- streaming desacoplado del gameplay;
- audio focus;
- telemetría autenticada;
- crash/hang markers;
- accesibilidad;
- localización.

## Lo que no se simula

La publicación del catálogo de achievements/trophies, la distribución final de DLC/patches, los overlays de plataforma y la certificación formal no se falsifican dentro del repositorio. Esas funciones requieren el provider oficial y las herramientas autorizadas del fabricante.

Un build que pasa CI del repositorio no equivale por sí solo a una certificación de consola.

## Gate final

El producto solo puede entrar en Release Candidate cuando exista un build UE5.6 real cocinado para la plataforma, ejecutado en hardware objetivo, con pruebas de larga duración, memoria, streaming, suspend/resume, input, audio, online, achievements/entitlements, localización, accesibilidad, crash/hang y cumplimiento de la documentación vigente del fabricante.
