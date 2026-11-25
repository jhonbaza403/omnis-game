# Omnis — Realismo visual, 4K e interfaz premium

## Dirección artística

La meta es una fantasía oscura cinematográfica con materiales, anatomía, iluminación y naturaleza físicamente creíbles. **No usar estética de cómic, cel-shading ni acabado plástico.** La fantasía procede de las fuerzas elementales, las criaturas y la historia; los objetos deben conservar escala, peso, desgaste y lógica material.

La calidad visual no se obtiene con una sola variable de resolución. Requiere mallas, UV, texturas, materiales PBR, rig, animación, iluminación, composición, posprocesado, streaming y presupuesto de rendimiento coherentes.

## Objetivo de renderizado

- Resolución de referencia: 3840×2160 (4K UHD), con resolución escalable.
- UE 5.8: evaluar Nanite, Lumen, Virtual Shadow Maps, Virtual Textures y escalado temporal por compatibilidad y coste real.
- Mantener perfiles Cinematic, Ultra, High, Balanced y Performance.
- 60 FPS en 4K es un objetivo para hardware de referencia de gama alta, no una promesa universal. Publicar la especificación de CPU/GPU, capturas de frame time y consumo de memoria antes de certificarlo.
- No activar funciones costosas a ciegas. Comparar resultados visuales, artefactos, VRAM, CPU/GPU y estabilidad en una ruta de benchmark repetible.

## Personajes

1. Malla humana con proporciones, rostro y silueta creíbles; evitar el cubo de sustitución como resultado final.
2. Materiales distintos para piel, tela, cuero, metal, barro y humedad; roughness y desgaste con variación.
3. Esqueleto y animaciones de locomoción, giros, frenado, saltos, interacción y combate con anticipación, impacto y recuperación.
4. Contacto de pies con terreno, reacciones de impacto y expresividad facial/corporal.
5. LODs, materiales, colisiones y streaming revisados en juego.
6. Cosméticos originales, legibles y sin ventajas competitivas comprables.

## Naturaleza y mundo

- Terreno con estratos, pendientes transitables, roca, suelo, costas y transiciones de agua coherentes.
- Vegetación adaptada a cada bioma, densidad irregular, variación de escala/rotación y viento; evitar patrones repetitivos visibles.
- Fauna con animaciones y comportamiento acordes al entorno.
- Clima, hora del día, niebla y efectos ambientales sin ocultar injustamente enemigos ni objetivos.
- Instancing, HLOD, World Partition, streaming, colisiones y densidad escalable para sostener el rendimiento.
- No declarar un bosque, río o región terminados por el mero hecho de haber colocado meshes de prueba.

## Interfaz 4K y accesibilidad

La interfaz debe ser responsiva, no una imagen fija de 3840×2160. El HUD prioriza salud, habilidades, objetivo y contexto de interacción; los menús mantienen una jerarquía visual consistente, estados de foco claros y navegación completa con teclado/ratón y mando.

- Probar 4K, 1440p, 1080p, 720p, ventana y ultrawide.
- Tipografía e iconos vectoriales o de alta resolución, contraste suficiente, áreas seguras y escala configurable.
- Opciones para subtítulos, tamaño de texto, daltonismo, remapeo de controles, reducción de movimiento y vibración.
- Animación y desenfoque decorativos nunca deben perjudicar lectura, accesibilidad o tiempo de respuesta.

## Evidencia de aceptación

Un activo progresa por estados separados: planned → source-authored → imported-in-editor → integrated-in-level → runtime-tested → performance-verified. El catálogo o una imagen conceptual no demuestra que un activo exista como .uasset ni que funcione en una partida.

La primera certificación visual debe incluir una región jugable, un héroe riggeado, terreno y vegetación, enemigo, jefe, HUD y menús, guardado/carga, captura 4K y prueba de rendimiento con hardware identificado. Las builds y las pruebas dentro de Unreal Engine 5.8 son necesarias para cerrar estos criterios.