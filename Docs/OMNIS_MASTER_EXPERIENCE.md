# OMNIS: EL ÚLTIMO CICLO — ESPECIFICACIÓN MAESTRA

## Objetivo

Omnis es un mundo abierto social de aventura, creatividad y competición. El jugador puede explorar, jugar, construir, personalizarse, compartir, competir, progresar y dejar consecuencias en el mundo.

## Bucle principal

EXPLORAR → JUGAR → CREAR → PERSONALIZAR → COMPARTIR → COMPETIR → PROGRESAR → CAMBIAR EL MUNDO

## Pilares

1. Identidad visual: dirección estilizada y cinematográfica con regiones, puntos de interés, iluminación dinámica, clima y VFX.
2. Personajes carismáticos: avatar, atuendos, cabello, emotes, mascotas y estado de personalización.
3. Progresión gratificante: experiencia, niveles y recompensas cosméticas sin ventajas competitivas comprables.
4. Construcción: piezas en cuadrícula, límites, validación de colocación y autoridad del servidor.
5. Cooperación: grupos de hasta ocho miembros y estado de preparado.
6. Social: amistades, grupos, espacios compartidos y controles de seguridad.
7. Competitivo: rating, divisiones, victorias/derrotas y futuras tablas de clasificación.
8. Narrativa: decisiones, facciones, relaciones y consecuencias persistentes.
9. El Ciclo: transición Awakening → Crisis → Collapse → Nexus → Rebirth con estado heredable.
10. Mundo abierto: World Partition real de Unreal Engine 5.6.
11. Creadores: borradores, elementos, publicación y valoración de contenido generado por usuarios.
12. Seguridad: bloqueo, reportes, límites de contexto y permisos de interacción.

## Arquitectura

Core contiene identidad, progreso, ciclo, persistencia y eventos. Gameplay contiene personaje, interacción, construcción, combate, traversal, inventario y vehículos. World contiene World Partition, terreno, clima, streaming, vida y estado mundial. Online contiene cuentas, amigos, matchmaking, grupos, ranking, creadores y seguridad. Narrative contiene decisiones, diálogo, reputación y consecuencias. UI expone HUD, inventario, menús, Nexo y accesibilidad.

## Definition of Done

- El proyecto abre con Unreal Engine 5.6.
- La configuración apunta a Omnis_OpenWorld.
- El materializador utiliza el template Open World real cuando Unreal está disponible.
- Los OBJ del repositorio son importados como assets nativos por el editor.
- Existe una capa de materiales premium PBR creada por Unreal.
- El personaje se mueve, explora, interactúa, combate y construye.
- La construcción es validada y limitada por el servidor.
- El jugador recibe XP y recompensas.
- El avatar puede equipar cosméticos desbloqueados.
- Un grupo puede formarse y usar ready-state.
- Una partida puede actualizar rating.
- Una creación puede publicarse y recibir likes.
- Un jugador puede bloquear y reportar.
- El ciclo conduce al Nexo y al renacimiento.
- La prueba de integración cubre estos contratos.
- CI debe validar Python, servicios, materialización Unreal, automatización, build, package y smoke test del servidor.
- No se aceptan binarios ficticios, servicios externos simulados como producción ni migraciones de datos inventadas.

## Estado real

Los assets nativos UAsset y el mapa UMAP no se deben fabricar como texto. Se generan dentro de Unreal Engine por CI mediante Tools/Editor/MaterializeOmnisPremiumWorld.py. El proyecto ya contiene las fuentes OBJ y el generador; la materialización real depende de que el runner tenga Unreal Engine 5.6 instalado y accesible mediante UE_ENGINE_ROOT.
