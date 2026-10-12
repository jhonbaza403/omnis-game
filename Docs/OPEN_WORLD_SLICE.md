# Omnis — primera región de mundo abierto

## Alcance materializado en código

La escena de inicio conserva el Nexo como punto de orientación y amplía el terreno de una arena pequeña a un territorio continuo generado de forma determinista al arrancar.

- Terreno base de aproximadamente 600 × 600 unidades, con silueta de montañas en el horizonte; el límite visual no equivale a streaming de mundo.
- Tres rutas legibles desde el Nexo hacia una región verde, unas ruinas de cresta y un monolito de marisma.
- Aldea prototipo de ocho casas alrededor de una plaza y un faro luminoso.
- Río de éter con dos cruces, arbolado de frontera, rocas, árbol ancestral, torre rota y una aguja distante como puntos de orientación.
- Generación con semilla fija para mantener posiciones estables entre ejecuciones.

## Estado honesto

Esto constituye una **primera región abierta de prototipo**, no el mundo final ni un AAA terminado. Los edificios y el terreno se construyen con primitivas y materiales procedurales: sirven para probar escala, navegación y composición, pero necesitan modelos, animaciones, audio final, NPC, misiones, colisiones revisadas y dirección artística profesional.

La generación actual crea muchos GameObjects al inicio y no implementa streaming por celdas, LOD de vegetación, guardado de cambios del mundo ni presupuesto de rendimiento. Antes de expandir a continentes, debe medirse el tiempo de arranque y FPS en el Editor y en un build real; después conviene migrar a Terrain/meshes, GPU instancing y carga por regiones.

## Criterios de aceptación pendientes

1. Abrir UnityProject/ con la versión objetivo aprobada y confirmar que paquetes y scripts importan sin errores.
2. Ejecutar la escena y recorrer las tres rutas a pie; verificar que puentes, casas, rocas y árboles no bloqueen injustamente al jugador.
3. Verificar combate, reliquias, Nexo y HUD después de expandir el terreno.
4. Ejecutar PlayMode tests y build Linux; registrar resultados reales.
5. Medir rendimiento en hardware objetivo y establecer mínimos de FPS/memoria antes de añadir más densidad.
6. Implementar streaming y persistencia por región antes de afirmar que el universo completo es un mundo abierto de gran escala.

## Coordenadas orientativas

- Nexo: alrededor de (0, 0, 10).
- Aldea Wayfarer: alrededor de (112, 0, 88).
- Árbol ancestral y torre rota: hacia (-118, 104) y (-158, 124).
- Monolito de marisma: alrededor de (88, -112).
- Aguja distante: alrededor de (176, -158).
