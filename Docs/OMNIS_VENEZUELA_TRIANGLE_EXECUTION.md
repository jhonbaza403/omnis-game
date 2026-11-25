# OMNIS — Plan maestro del mundo moderno venezolano

**Motor objetivo:** Unreal Engine 5.8  
**Rama de trabajo:** la PR de auditoría/integración existente  
**Estado del territorio:** catálogo de diseño; todavía no es un mapa jugable materializado.

## 1. Decisión de arquitectura

El triángulo Caracas–La Guaira–Barlovento se incorpora como un **World Pack moderno e independiente**. No sustituye ni modifica silenciosamente las doce regiones elementales y el progreso de *El Último Ciclo*. Ambos contenidos pueden coexistir en el producto mediante paquetes de mundo, reglas, personajes y misiones identificados por datos.

La línea técnica principal sigue siendo Unreal Engine 5.8 y C++. El servidor dedicado es la autoridad de inventario, dinero, misiones, daños persistentes, territorio y estado económico. Los clientes reciben solo el estado público que necesitan para representar el mundo. No se incorpora SpatialOS ni un segundo backend hasta que las pruebas de carga demuestren que la arquitectura de servidor/región actual no alcanza.

## 2. Los cinco pilares y su estado honesto

| Pilar | Base que ya existe | Trabajo que falta para considerarlo terminado |
|---|---|---|
| NPC con memoria y conversación generativa | Identidad persistente, necesidades, horarios, relaciones, memoria acotada, rumores por misión y recuperación social | Adaptador generativo asíncrono local/nube, contexto narrativo resumido, privacidad, límites de coste, moderación, diálogo contextual y simulación de población fuera de pantalla |
| Destrucción y reparación persistentes | Salud/daño de entidades y estado persistente de mundo | Componentes de estructura por piezas, Geometry Collections/Chaos en activos preparados, snapshots por edificio, reconstrucción, cuadrillas, persistencia regional y pruebas de replicación |
| Economía orgánica | Nuevo subsistema de mercado: existencias, demanda, precio entero, varias monedas, compra, venta, trueque, saldo y catálogo inicial | Transacciones solicitadas por RPC desde clientes, identidad online estable, UI de mercado, impuestos/corrupción, propiedades, transporte de mercancía y shocks automáticos conectados a incidentes de rutas |
| Interiores accesibles | Modelos fuente de algunos interiores, streaming/world-partition como dirección de arquitectura | Registro de entradas, tipos de edificio, generador por reglas, navegación interior, persistencia, prueba de continuidad y medición real del 80 % |
| Modding nativo | Límites de contenido y contratos de datos existentes | Manifiestos/versiones, validación, firma, permisos, sandbox de datos, herramientas, publicación, compatibilidad de servidor y moderación de contenido |

El 80 % de interiores es un **objetivo medible**, no una característica presente. Se medirá como edificios accesibles / edificios incluidos en el inventario del World Pack; se exige además entrada y salida sin pantalla de carga en el hardware objetivo. No se debe contar una puerta decorativa como interior accesible.

## 3. Mapa y conexiones

Las anclas del archivo `venezuela-triangle-v1.json` son coordenadas relativas del juego, no coordenadas topográficas ni distancias reales. Deben sustituirse por terreno, carreteras, hidrología, límites y navegación validados antes de producir niveles.

### Fase 1 — primer vertical slice

**Caracas Este → Guarenas → Guatire → Caucagua → Río Chico → Higuerote.**

Debe incluir un barrio vertical compacto, una arteria urbana, transición periurbana, ruta húmeda rural, un pueblo fluvial y un hub costero. Esta ruta permite validar conducción, barro/lluvia, combustible, mercados, NPC, una misión con consecuencias y una muestra de interiores sin producir todo el país.

### Fase 2 — puerto y cordillera

La Guaira, aeropuerto, muelles, aduanas, autopista de túneles/viaductos y carretera vieja. La importación de bienes debe alimentar mercados regionales; las interrupciones de puerto o carretera deben poder modificar el inventario disponible de los mercados conectados.

### Fase 3 — costa y navegación

Naiguatá, Todasana, Chuspa, manglares y conexión marítima hacia Higuerote. Embarcaciones, oleaje, tormentas, visibilidad, calado y rutas costeras requieren física y navegación probadas, no solo puntos de mapa.

## 4. Economía: contrato de simulación

El nuevo `UOmnisEconomySubsystem` mantiene mercados por `MarketId + ItemId`, región y moneda. El precio se calcula con aritmética entera en unidades menores de la moneda para evitar deriva de coma flotante. El catálogo inicial del primer corte contiene mercados de Caracas, Guarenas–Guatire, Caucagua, Río Chico e Higuerote; la semilla del puerto de La Guaira solo se registra al solicitar la fase 2. El índice reacciona a existencias/demanda y tiene límites para evitar precios infinitos o gratuitos.

- Compra: comprueba existencias, saldo y capacidad del inventario antes de debitar fondos.
- Venta: comprueba inventario y que el saldo no desborde el máximo antes de acreditar fondos.
- Trueque: exige que la cesta ofrecida cubra el valor de la solicitada y que ambas referencias compartan moneda.
- Los mercados pueden ser públicos o restringidos. Solo las cotizaciones públicas se replican al actor mundial; los saldos de las cuentas no se transmiten a todos los clientes.
- Las rutas del triángulo ya se resuelven a sus mercados conectados mediante `ApplyRouteSupplyShock`. Las definiciones de misión pueden aplicar shocks de oferta/demanda al fallar y la misión de recuperación puede declarar el shock inverso. La automatización de eventos meteorológicos/aduaneros y la simulación de duración/expiración siguen pendientes.
- Los valores de semilla son unidades de economía ficticia del juego, no precios económicos reales.

## 5. IA social y memoria

La IA no debe llamar un LLM en cada Tick. La simulación de necesidades, agenda, navegación, percepción y relaciones sigue siendo determinista y local al servidor. Un adaptador generativo podrá preparar respuestas cuando ocurra un evento conversacional, usando un resumen de memoria limitado, identidad de personaje, relación, conocimiento permitido y contexto de misión.

El modelo solo propone texto/intenciones permitidas. Nunca decide dinero, inventario, daño, cumplimiento de misión, propiedad o autoridad policial. Esas acciones pasan por reglas C++ validadas por el servidor. Se requiere proveedor intercambiable local/nube, tiempo límite, caché, fallback escrito, límites por jugador y filtros de contenido. No se guardan secretos de proveedor dentro del SaveGame.

## 6. Destrucción y reconstrucción

No se debe intentar voxelizar todo el mapa ni simular cada escombro como actor persistente. La propuesta es híbrida:

1. Estructuras críticas y edificios emblemáticos: activos preparados con Chaos/Geometry Collections y piezas de daño con IDs estables.
2. Edificios comunes: estados por componentes (fachada, puerta, ventana, cubierta, soporte) y niveles de deterioro.
3. Persistencia: guardar eventos/estado agregado por `BuildingPersistentId`, celda y versión; no guardar cada fragmento físico transitorio.
4. Reparación: cuadrillas/NPC con inventario de materiales, tiempo de trabajo y contratos. El paso del calendario produce recuperación gradual; sabotajes o escasez de materiales la retrasan.
5. Multiplayer: el servidor replica cambios importantes y el cliente reconstruye efectos visuales localmente.

El código actual todavía no acredita destrucción estructural persistente ni reparación de edificios.

## 7. Interiores, streaming y verticalidad

Cada edificio del inventario del mundo debe indicar categoría, número de pisos, entradas, rutas verticales, tipo interior, ocupación, nivel de accesibilidad y memoria estimada. La generación por reglas debe reutilizar módulos de apartamentos, oficinas, tiendas, garajes, hospitales, almacenes, alcantarillas y azoteas. Los interiores singulares se diseñan a mano.

El streaming usa celdas y presupuestos de memoria; la simulación de NPC fuera de radio usa LOD de comportamiento, no navegación completa. El objetivo de continuidad se certifica midiendo cruces de umbral, visibilidad, memoria, FPS y ausencia de pantalla de carga entre exterior/interior. No se declara alcanzado antes de esas pruebas.

## 8. Modding y contenido de usuarios

Los primeros paquetes deben ser **data-only**: misiones, reglas de economía, diálogos, vehículos autorizados, decoración, mapas de eventos y presets de servidor. Cada paquete requiere ID, versión, dependencias, manifiesto, hash/firma, capacidades permitidas, límites de tamaño y validador. Los servidores deciden qué paquetes aceptan. No se ejecuta código nativo arbitrario ni scripts descargados sin sandbox y revisión.

Las herramientas de autor deben permitir validar el paquete antes de publicarlo y detectar IDs duplicados, dependencias ausentes, referencias rotas y versiones incompatibles.

## 9. Criterios de salida por fase

| Gate | Evidencia requerida |
|---|---|
| Compilación | UHT/UBT Editor, Game y Dedicated Server con UE 5.8 |
| Mundo | Mapa materializado, World Partition, navegación terrestre/acuática, colisiones y puntos de streaming |
| Conducción | Pruebas por superficie seca/mojada, pendiente, barro, combustible y daños |
| Economía | Compra/venta/trueque, simultaneidad, falta de saldo/stock, reintento y guardado/carga |
| NPC | Rutinas, memoria, rumor, recuperación, fuera de pantalla y límites de población |
| Destrucción | Daño, persistencia tras reinicio, reconstrucción, cliente/servidor y rendimiento |
| Interiores | Numerador/denominador de accesibilidad, cruce exterior/interior y memoria |
| Mods | Firma, manifiesto, validación, permisos, instalación y rechazo de paquete inválido |
| Rendimiento | Perfil de población, streaming, red, CPU/GPU y memoria en hardware mínimo definido |

**Regla de producción:** una función solo cambia de “planificada” a “integrada” cuando tiene implementación, prueba y evidencia en Unreal 5.8. El catálogo y la validación JSON no sustituyen un mapa, assets, compilación o prueba jugable.

## Estado de esta iteración — código en revisión, no mapa certificado

- Economía regional: implementada como base C++ de servidor para cotizaciones, precios por oferta/demanda, compra/venta/trueque, multimoneda y guardado versionado. El seed inicial abre fase 1; La Guaira es fase 2.
- Crisis de rutas: las rutas del catálogo se conectan a mercados mediante `ApplyRouteSupplyShock`. Una misión puede declarar rutas y artículos afectados al fallar; una misión de recuperación puede declarar la reversión.
- Daño de estructuras: existe daño por secciones etiquetadas con replicación y snapshot persistente por estructura. Chaos/Geometry Collections, escombros físicos y cuadrillas de reparación aún requieren activos y runtime tests.
- Mods: existe un manifiesto de autoría data-only y un ejemplo de eventos de mercado. No está firmado y no puede instalarse en producción; el workflow valida sus referencias y niega ejecución arbitraria de código.
- Daño, economía y mercado ya cuentan con pruebas escritas; hasta que corran en Unreal 5.8 no son evidencia de ejecución nativa.

## Registro de interiores y medición del objetivo 80 %

El archivo `Content/Omnis/Data/building-interiors-v1.json` define 20 edificios de producción del World Pack. Dieciséis son el objetivo accesible para el conjunto completo; en la fase 1 se exigen al menos 13 de 16. Cada edificio tiene ID persistente, subregión, número de pisos, punto de entrada, tipos de sala, rutas verticales y estado de materialización/prueba.

El estado inicial es explícito: cero edificios materializados, cero probados y ratio medido nulo. Solo se contará un edificio cuando exista una entrada funcional, interior navegable, continuidad de streaming y prueba de transición exterior/interior. Una puerta, fachada o modelo OBJ no cuenta como interior accesible.

## Conversación generativa: contrato para la siguiente integración

Los recuerdos y rumores deterministas que existen ahora son el contexto de la conversación, no un LLM. El próximo adaptador de diálogo deberá:

- recibir un resumen acotado del personaje, necesidades, objetivo actual, relación y recuerdos relevantes;
- operar asíncronamente, con timeout, caché, presupuesto por jugador/NPC y fallback de diálogo autorado;
- aceptar proveedores locales o remotos intercambiables, sin fijar una marca de IA como identidad del producto;
- devolver texto/intención conversacional limitada; no puede editar saldo, inventario, misiones, daño, reputación, propiedad ni estado de red;
- validar salida, longitud, lenguaje y límites de contenido antes de mostrarla;
- evitar secretos de proveedor en los datos de partida y mantener el servidor como autoridad del mundo.

No hay todavía un proveedor LLM operativo integrado en la rama. No declarar conversaciones generativas hasta que exista proveedor, pruebas de coste/latencia y una sesión real en Unreal.
