# Sistema de combate original: Paso de Grieta y Pulso de Fractura

## Propósito

La identidad de combate de Omnis se construye alrededor de la **Resonancia**: cada impacto cuerpo a cuerpo confirmado recupera energía para una descarga de área. La evasión, llamada **Paso de Grieta**, es un desplazamiento breve con una ventana de invulnerabilidad acotada. Son mecánicas originales del proyecto; las referencias externas sirven para estudiar ritmo, legibilidad y respuesta, no para copiar personajes, animaciones, historias ni recursos.

## Controles del prototipo

- **F** o clic izquierdo: golpe cuerpo a cuerpo (la cámara debe tener el cursor capturado).
- **Ctrl izquierdo**: Paso de Grieta; usa WASD para orientar la evasión.
- **Q**: Pulso de Fractura cuando la Resonancia llega a 100.
- **WASD**: movimiento; **Mayús izquierdo**: correr; **Espacio**: salto.
- **E**: interactuar con fragmentos y el Obelisco.
- **R**: iniciar un nuevo ciclo tras activar el Nexo; **Esc**: pausa.

## Reglas iniciales

- Cada golpe que alcanza un centinela aporta 28 puntos de Resonancia, hasta 100.
- El Pulso consume toda la carga y aplica 48 de daño a centinelas dentro de 7,5 unidades.
- El Paso de Grieta dura 0,18 s, recorre aproximadamente 4,2 unidades y tiene 0,9 s de enfriamiento.
- La invulnerabilidad solo existe durante esa breve ventana; no se conserva al terminar la evasión.
- Las cifras son valores iniciales de prototipo y deben equilibrarse mediante pruebas de juego, no tratarse como balance final.

## Criterios de aceptación

1. La escena crea `OmnisResonanceCombat` en el jugador en el arranque.
2. Un golpe cuerpo a cuerpo que encuentra un centinela carga Resonancia; un golpe al aire no la carga.
3. Q con carga insuficiente informa del requisito sin gastar energía.
4. Q con carga completa consume la carga y daña a cada centinela dentro del radio. Los colliders múltiples del mismo enemigo deberán deduplicarse si el prefab crece.
5. Ctrl izquierdo inicia la evasión y bloquea el daño solo durante la ventana configurada.
6. La pausa detiene la entrada y no permite activar habilidades.
7. La validación de archivos no equivale a compilación: Unity Editor debe importar el proyecto, compilar scripts, ejecutar PlayMode tests y producir un build reproducible antes de considerar certificada la versión.

## Próxima iteración

- Mostrar barra/medidor de Resonancia en el HUD.
- Añadir señales audiovisuales y animaciones de anticipación/impacto con recursos originales.
- Evitar doble aplicación de daño si un centinela utiliza varios colliders.
- Añadir pruebas PlayMode para carga, consumo, radio, evasión e invulnerabilidad.
- Equilibrar valores con telemetría de duración de encuentros y daño recibido.
