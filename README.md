# OMNIS — El Ciclo del Sin-Ciclo

Proyecto de videojuego Unreal Engine 5 con una arquitectura C++ modular y una primera implementación funcional de extremo a extremo.

## Vertical slice implementado

BOOT → PRIMER CICLO → MUNDO → EXPLORACIÓN → NPC → DECISIÓN → COMBATE → FIN DEL CICLO → NEXO → MEJORA → NUEVO CICLO

El ciclo de muerte ya conecta la transición al Nexo, la generación de un punto de mejora permanente, la mejora de vitalidad y el renacimiento del jugador.

## Módulos runtime

Core, Gameplay, World, Narrative, AI, Multiplayer, Online, UI, Audio, Camera, Data, Systems y Developer disponen de implementación C++ concreta.

## Infraestructura

- Seis plugins Omnis con módulos Unreal reales.
- Herramientas de validación, datos, mundo, build y automatización.
- Suite de pruebas Python por dominio.
- Pipeline GitLab CI para validación estática y pruebas.
- Git LFS para activos binarios.
- Manifiestos de contenido para mantener contratos explícitos antes de incorporar assets binarios.

## Reglas

No se crean migraciones ficticias, sistemas duplicados ni carpetas vacías utilizadas como sustituto de código.

Las integraciones online representan servicios locales y contratos de runtime hasta que exista un backend externo configurado; no se simulan respuestas de un proveedor inexistente.
