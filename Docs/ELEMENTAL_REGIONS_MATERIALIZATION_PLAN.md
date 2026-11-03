# Omnis — Plan de materialización de las 12 regiones elementales

## Alcance de este cambio

El archivo `Content/Omnis/Data/elemental-regions-v1.json` establece un catálogo de diseño versionado: 12 regiones, identidad visual, cultura, mecánica ambiental, perfil de combate y jefe. También define una distribución de 134 subregiones por región, para un total planificado de 1.608.

**Estado honesto:** este catálogo es dato de diseño. No significa que existan 1.608 mapas, que los jefes estén implementados o que el juego esté listo para jugar. Los mapas y assets nativos deben generarse y validarse en Unreal Engine 5.8.

## Convención de subregiones

El identificador de una subregión debe ser estable y no depender del nombre traducido:

`<region_id>.<tier>.<number>`

Ejemplo: `ice.initiation.001`, `ice.mythic.133`, `time.convergence.134`.

Los nombres visibles se guardan como datos de localización, no como identificadores de persistencia. Cada entrada materializada debe registrar como mínimo:

- ID estable, región y nivel de progresión.
- Referencias a mapa/World Partition y puntos de interés existentes.
- Objetivos, encuentros, recursos y reglas de reaparición.
- Requisitos de finalización, recompensas y consecuencias narrativas.
- Presupuesto de rendimiento, streaming y estado de validación.
- Estado explícito: `planned`, `authored`, `materialized`, `tested` o `playable_verified`.

No se debe elevar el estado de una región sin evidencia reproducible.

## Primer vertical slice recomendado

No intentar materializar las 1.608 subregiones a la vez. Construir una ruta completa y comprobable en la región **Valle del Despertar**:

1. Una zona de inicio pequeña con navegación, colisiones y puntos de referencia legibles.
2. Una mecánica de restauración de raíces que abra una ruta alternativa.
3. Un encuentro básico con telegráficos, daño, bloqueo/esquiva y reinicio seguro.
4. Un objetivo narrativo con una decisión que cambie un estado observable.
5. Un punto de retorno al Nexo, guardado/carga y una recompensa cosmética.
6. Pruebas automatizadas de IDs de datos, referencias de assets, objetivos y estados de progresión.
7. Prueba manual en el editor y paquete ejecutable antes de marcarlo como jugable.

## Integración con Unreal

- Leer y validar el JSON desde una herramienta de editor o convertirlo en Data Assets dentro de Unreal; no simular archivos `.uasset` con texto.
- Mantener las referencias a mapas y assets nativos separadas del catálogo conceptual.
- Añadir comprobaciones de esquema y de IDs únicos a CI antes de ejecutar materialización.
- Confirmar que el runner tiene Unreal Engine 5.8 accesible mediante la configuración documentada antes de declarar éxito de materialización.
- Mantener la autoridad del servidor para resultados competitivos y cambios persistentes del mundo.

## Calidad y equidad

- La dificultad debe venir de patrones, objetivos y coordinación, no de ventajas compradas.
- Las recompensas competitivas deben ser cosméticas o de prestigio; normalizar estadísticas en partidas clasificatorias.
- No usar apuestas con dinero real ni cajas de botín pagadas aleatorias. Los torneos pueden usar premios patrocinados o recompensas virtuales sin valor monetario, sujeto a las reglas aplicables.
- Ofrecer opciones de accesibilidad para contraste, tamaño de subtítulos, reasignación de controles y señales visuales/sonoras.

## Criterios de aceptación

- El JSON se parsea correctamente y sus cantidades suman 1.608.
- Hay exactamente 12 IDs de región únicos.
- La distribución por niveles suma 134 por región.
- Ningún documento afirma que los mapas o jefes están listos solo por aparecer en el catálogo.
- La primera región solo se declara jugable después de ejecutar y documentar la prueba dentro de Unreal.
