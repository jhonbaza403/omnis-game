# Omnis — principios originales inspirados en grandes mundos abiertos

Este documento convierte referencias del género en objetivos de diseño propios. Omnis no reutiliza personajes, mapas, misiones, código, diálogos, música, nombres de sistemas protegidos ni assets de otros juegos.

## Identidad de Omnis

**Un mundo abierto de fantasía primordial donde la Resonancia cambia el entorno, las decisiones dejan huella y explorar verticalmente revela rutas, secretos y amenazas.** La progresión gira en torno al Nexo y los ciclos del mundo.

## Qué aprender de cada referencia y cómo traducirlo

| Referencia | Principio | Traducción original a Omnis |
|---|---|---|
| Elden Ring | Exploración no lineal y jefes memorables | Ruinas opcionales, rutas ocultas y jefes regionales con recompensas alternativas |
| Red Dead Redemption 2 | Mundo con ritmo y personajes vivos | Ciclo día/noche, atmósfera cambiante, horarios y reacciones de NPC por etapas |
| Zelda: Tears of the Kingdom | Verticalidad y creatividad física | Escalada en rutas diseñadas, planeo/descenso y construcción de puentes y balsas con materiales |
| Grand Theft Auto V | Ciudad con actividades conectadas | Ciudad fantástica por distritos, mercados, encargos, carreras de monturas y encuentros reactivos |
| Cyberpunk 2077 | Identidad de ciudad y narrativa personal | Distritos con cultura propia, artefactos antiguos, decisiones y reputación |
| The Witcher 3 | Contratos con dilemas | Caza de criaturas, pistas y elecciones que cambian el destino de aldeas o facciones |
| Ghost of Tsushima | Lectura visual del paisaje y combate legible | Viento y partículas como guía opcional, duelos claros y sigilo original |
| Minecraft | Recolección y construcción | Recursos, recetas y construcción limitada por terreno y materiales |
| Assassin's Creed Shadows | Estaciones y sigilo | Variaciones estacionales en regiones seleccionadas y rutas de infiltración alternativas |
| Crimson Desert | Combate físico espectacular | Combos con peso, ataques telegrafiados e interacción con el terreno |
| Palworld | Criaturas aliadas | Criaturas originales que se vinculan y ayudan en tareas con límites claros |
| Soulmask | Supervivencia y grupo | Refugios, recursos, expediciones y habitantes aliados como capa opcional |
| Gothic Remake | Facciones y reputación | Acceso, precios y ayuda que cambian según las acciones del jugador |
| Kingdom Come: Deliverance 2 | Coherencia social | Rutinas sencillas de NPC, simuladas por proximidad para controlar el coste |
| Skyrim | Libertad de rol | Especializaciones combinables, habilidades y líneas de facción independientes |
| Fallout 4 | Construcción persistente | Mejoras de refugios, almacenamiento y cambios del mundo que se guardan |
| Dragon's Dogma 2 | Compañeros y encuentros emergentes | Compañeros con roles tácticos y encuentros según región/ruta |
| Horizon Forbidden West | Criaturas con puntos débiles | Bestiario original, señales legibles, partes recuperables y contramedidas |

## Plan por fases

### Fase A — primera región
- Ciclo visual día/noche y estados de atmósfera/clima.
- Rutas con puntos de interés, secretos y encuentros opcionales.
- Interacciones, combate, reliquias y progreso local.
- Primer contrato de investigación con pistas y una decisión significativa.

### Fase B — simulación del mundo
- NPC por arquetipo/horario y reputación persistente de la aldea.
- Inventario, recursos, recetas y dos construcciones de travesía.
- Guardado de cambios relevantes y eventos regionales.
- Primera criatura aliada, tras validar IA y rendimiento.

### Fase C — expansión
- Streaming de regiones, LOD/culling, perfiles de rendimiento y herramientas de autoría.
- Más biomas, asentamientos, mazmorras, jefes, contratos y facciones.
- Monturas, viaje rápido coherente con la ficción y travesía avanzada.
- Cooperativo de cuatro jugadores solo tras estabilizar el modo individual, autoridad de servidor y guardado.

## Implementado en esta iteración

Se añade un controlador inicial de ciclo y atmósfera: transiciones de iluminación entre día/noche y estados visuales de cielo despejado, bruma del Velo y tormenta de Grieta. No representa todavía lluvia física, estaciones, IA social ni streaming. La compatibilidad y ejecución deben confirmarse con el Editor de Unity de la versión elegida.

## Criterios de calidad
- Cada referencia se traduce a un sistema original; no se copian mapas, personajes, diálogos, música, misiones ni arte.
- Cada zona debe tener identidad, actividad y cambios significativos, no solo más kilómetros.
- Toda mecánica nueva debe tener pruebas, presupuesto de rendimiento y accesibilidad.
