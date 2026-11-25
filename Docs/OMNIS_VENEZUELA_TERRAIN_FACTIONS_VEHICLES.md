# OMNIS — Relieve, biomas, territorio y conducción del Triángulo venezolano

**Motor objetivo:** Unreal Engine 5.8  
**Paquete:** WorldPack.ModernVenezuela  
**Estado:** contratos de datos y validación estática; el mapa 3D requiere materialización y pruebas nativas.

## 1. Perfil altimétrico
Las altitudes se expresan en metros sobre el nivel del mar. Los perfiles emplean progreso normalizado 0–1, no GPS ni distancias geodésicas. Se modelan cuatro recorridos: valle de Caracas (~900 m) hasta la cresta de Waraira Repano (~2.700 m); descenso Caracas–La Guaira hasta 0 m; descenso progresivo Caracas Este–Guarenas–Guatire–Caucagua; y la llanura Caucagua–Río Chico–Higuerote entre 0 y 50 m. La interpolación permite alimentar pendiente, frenos, potencia, niebla y reglas de revelado.

**Conversión Unreal:** 1 m = 100 cm. El origen vertical y la escala se fijarán al materializar el mapa. Las cotas son objetivos de diseño, no un levantamiento topográfico. El perfil matemático no crea por sí solo montañas, barrancos, túneles ni viaductos: faltan heightmaps/Landscape, splines, colisiones, navegación y activos.

## 2. Biomas y superficies
Caracas diferencia asfalto desgastado, concreto, ladrillo expuesto y vegetación de montaña; La Guaira usa roca, concreto salino, asfalto y arena; Barlovento incorpora asfalto mojado, grava, barro profundo, suelo de manglar y canales. Cada superficie declara agarre, penalización mojada, resistencia a la rodadura, deformación potencial y corrosión marina. Las roderas registradas son datos de simulación; la deformación visual necesita Landscape/RVT o malla/material con render target. Shaders, foliage, Nanite y materiales finales siguen pendientes.

## 3. Streaming y revelado
La Guaira tiene gates de revelado al cruzar la salida del túnel o la cresta del Waraira Repano. Es una regla de diseño, no streaming operativo. Faltan Data Layers/World Partition Runtime Cells, HLOD, volúmenes, presupuestos y perfilado de hitching.

## 4. Facciones
El catálogo define cuatro territorios y cinco facciones ficticias, incluida una red de seguridad común. Reputación, contacto local y calor territorial determinan acceso y respuesta abstracta. Se necesita enlazar esas reglas a AIControllers, percepción, navegación, spawn, replicación y pruebas. El proveedor LLM no puede mutar saldo, inventario, reputación ni misiones; el servidor valida cualquier consecuencia.

## 5. Vehículos
Los perfiles abarcan motocicleta, sedán, 4x4, peñero y lancha rápida. Se define combustible, desgaste de motor/transmisión/suspensión/frenos/neumáticos/refrigeración, temperatura, fading, alineación y carga. El sistema de condición no sustituye rigs Chaos: suspensión por rueda, diferencial real, caja 4L, vuelco, snorkel, flotabilidad multipunto, oleaje y control náutico requieren rigs, activos y tuning.

## 6. Operación narrativa
operation-cargo-echo-v1 describe una operación de envío disputado: interacción portuaria, elección de ruta según estado/clima/vehículo, transferencia rural opcional y cierre comercial en Caracas. El grafo incluye referencias a economía, facciones y estado persistente, pero necesita convertirse en PrimaryDataAssets y enlazarse al sistema de misiones existente.

## 7. Criterios para certificar
1. UHT/UBT en UE 5.8 para Editor, Game y Dedicated Server. 2. Crear Landscape/heightmaps, carreteras, túneles, viaductos, Data Layers y HLOD. 3. Implementar materiales, máscaras de salitre/barro y RVT. 4. Montar y calibrar Chaos rigs para moto, sedán/4x4 y embarcaciones. 5. Enlazar facciones y respuesta IA, con persistencia/replicación. 6. Convertir la misión a assets y probar éxito/fallo/recuperación. 7. Ejecutar Automation Tests, cook/package, cliente-servidor y perfil de rendimiento.

## 8. Puente LLM recibido
El ejemplo HTTP adjunto no se copia literalmente: exponer ApiKey en propiedades públicas y permitir que el modelo devuelva ReputationDelta o acciones de juego le concede autoridad indebida. La arquitectura OMNIS permite al LLM proponer diálogo e intención conversacional enumerada, pero las mutaciones del mundo siguen siendo deterministas y validadas en servidor; faltan moderación, rate limits, cancelación, cache, circuit breaker y fallback probado.


## 9. Pendiente de ruta y temperatura de frenos

El subsistema de terreno calcula el porcentaje de pendiente entre dos progresos del perfil cuando el caller aporta la distancia horizontal real del tramo, procedente de la spline/geometría de ruta. El Pawn expone `SetRoadGradePercent`: una subida aumenta el factor de carga térmica del motor y una bajada aumenta la generación térmica de frenos según pendiente, velocidad y carga. La carretera debe suministrar esa distancia; el progreso normalizado no determina por sí solo una pendiente física. Este modelo de juego necesita calibración con telemetría dentro de UE 5.8.
