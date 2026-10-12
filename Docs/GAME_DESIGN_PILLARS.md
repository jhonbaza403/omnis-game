# OMNIS: El Último Ciclo — visión jugable y pilares de diseño

## Identidad

Omnis es una aventura original de acción en tercera persona, fantasía oscura y ciencia mítica. La promesa central es explorar un mundo antiguo que cambia después de cada ciclo, dominar un sistema de combate táctico y reconstruir la verdad sobre el Nexo. Las referencias externas de este documento sirven para estudiar géneros y sensaciones de juego; Omnis no reutilizará personajes, nombres, mapas, música, diálogos, diseños ni recursos protegidos de esas obras.

## Pilares

1. **Combate físico, legible y cinematográfico**: ataques ligeros/pesados, bloqueo, esquiva, ventana de parry, interrupciones, enemigos con señales visibles y recuperación justa. La cámara debe mantener al jugador y la amenaza principal en pantalla.
2. **Encuentros con jefes por fases**: patrones reconocibles, cambios de arena y objetivos que se aprenden jugando; evitar depender únicamente de aumentar la salud del enemigo.
3. **Exploración con propósito**: rutas principales, desvíos, secretos, acertijos ambientales, reliquias y atajos persistentes. Cada región añade una regla de mundo distinta.
4. **Mundo reactivo**: clima, hora, fauna, recursos y asentamientos reaccionan a hitos narrativos. Persistencia y simulación se activan de forma escalonada para controlar coste de CPU/memoria.
5. **Progresión del Nexo**: al cerrar un ciclo se conserva un conjunto limitado de conocimientos, mejoras y desbloqueos; la muerte/reinicio modifica rutas y encuentros, no elimina arbitrariamente el progreso del jugador.
6. **Narrativa con consecuencias**: personajes con objetivos propios, decisiones legibles y cambios comprobables en regiones y misiones.
7. **Accesibilidad y control**: reasignación de teclas, sensibilidad de cámara, subtítulos, ayudas de combate opcionales, dificultad configurable y soporte de mando.
8. **Calidad técnica medible**: perfiles de rendimiento por plataforma; 4K y efectos de alto nivel son objetivos de perfil, no garantías universales. Priorizar frame pacing, lectura visual y tiempos de carga.

## Bucle principal

Explorar una región → encontrar señales y recursos → resolver una ruta/encuentro → luchar o negociar → activar una reliquia/Nexo → recibir una consecuencia persistente → desbloquear un camino o variación del siguiente ciclo.

## Sistemas para el primer vertical slice

- Controlador de tercera persona con aceleración, salto contextual donde proceda, esquiva y bloqueo.
- Cámara con seguimiento, colisión, recentrado configurable y encuadre de combate.
- Máquina de estados de jugador y enemigos; daño, stagger, invulnerabilidad breve y derrota.
- Un enemigo básico, un enemigo pesado y un encuentro de jefe de dos fases.
- Una arena, una ruta de exploración, un acertijo ambiental, tres reliquias y un punto Nexo.
- HUD con salud, recurso de combate, objetivo, interacción y estado del Nexo.
- Guardado/carga de progreso esencial y prueba de restauración.
- Audio placeholder legalmente limpio y eventos de impacto/ambiente.
- Pruebas automáticas de contratos + pruebas PlayMode; build descargable y smoke test manual documentado.

## Plan por fases

### Fase A — Proyecto reproducible
Editor/versiones de paquetes compatibles, escena principal en Build Settings, input configurado, importación limpia, validación CI y runner real.

### Fase B — Vertical slice jugable
Movimiento/cámara, combate completo básico, enemigos, arena, Nexo, HUD, guardado y pruebas. Esta fase debe compilar antes de expandir el mapa.

### Fase C — Producción de contenido
Herramientas de regiones, misiones, diálogo, puntos de interés, streaming y generación controlada de contenido. Los sistemas de mundo deben tener límites de presupuesto.

### Fase D — Pulido y lanzamiento
Perfiles gráficos, accesibilidad, optimización, pruebas de regresión, telemetría opcional respetuosa con privacidad, build firmado y distribución.

## Criterios de aceptación

- Un clon limpio del repositorio importa en la versión exacta del Editor sin errores de compilación.
- La escena principal inicia desde Build Settings sin intervención del editor.
- Las pruebas PlayMode pasan y generan resultados como artefacto.
- Un ejecutable de build se descarga, abre y completa el bucle principal.
- Un evaluador prueba movimiento, cámara, bloqueo/esquiva, daño, derrota, reliquias, Nexo, HUD y guardado/carga.
- El rendimiento se mide en hardware y resolución declarados.
- Cada asset externo tiene licencia/atribución registrada; no se incorporan rip-offs de las referencias.

## Referencias de inspiración de género

Consultar `GAME_REFERENCE_CATALOG.md` para la lista de juegos y sus usos de estudio. Las referencias describen objetivos abstractos de diseño, no tareas para copiar contenido.
