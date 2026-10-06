# Omnis — Estándar de producción AAA

Omnis se valida por capacidades funcionales medibles, estabilidad, rendimiento y evidencia de ejecución real. La cantidad de clases, directorios, manifests o jobs no se considera evidencia por sí sola.

## 1. Dirección visual

- Identidad artística propia.
- Art bible.
- Materiales PBR coherentes.
- Iluminación y composición consistentes.
- Personajes con rigs y deformación de calidad.
- Animación locomotion, combate y cinematics producida y revisada.
- VFX y audio sincronizados con gameplay.

Estado de ingeniería: la arquitectura admite estas capas, pero los assets finales siguen siendo producción de contenido.

## 2. Mundo

- World Partition y streaming medidos.
- Diseño de niveles por experiencia.
- Densidad de interacción controlada.
- NPC schedules y population budgets.
- Herramientas internas de world building.
- Validación de memoria por región.
- Streaming asíncrono de assets mediante StreamableManager.

Estado de ingeniería: el runtime de streaming existe y ahora distingue solicitud de carga de residencia efectiva. El profiling sobre mapas finales requiere hardware real.

## 3. Gameplay

- Una sola base de gameplay C++/UE5.6.
- Enhanced Input como ruta única de entrada.
- Cámara consistente.
- Combate legible y equilibrado.
- Traversal fiable.
- Construcción con reglas deterministas.
- Inventario y progresión persistentes.
- Economía y balance medidos.
- Loop de ciclo completo.

Estado de ingeniería: el sistema input común se ejecuta mediante Enhanced Input; los assets finales de input mapeable y perfiles de plataforma siguen siendo contenido/configuración de producción.

## 4. Online

- Identidad de usuario por provider.
- Perfiles cloud autenticados.
- Sesiones reales mediante IOnlineSession.
- Descubrimiento y unión mediante IOnlineSession.
- Matchmaking por playlist.
- Entitlements mediante IOnlineEntitlements.
- Achievements mediante IOnlineAchievements.
- Title content/update mediante IOnlineTitleFile.
- Persistencia cloud autoritativa.
- Recuperación de red.
- Rate limits.
- Telemetría autenticada.
- Observabilidad.

Estado de ingeniería: el código común está preparado; los providers propietarios y servicios de producción se validan en sus entornos autorizados.

## 5. Audio

- Estado de volumen maestro/música/SFX.
- Audio focus y recuperación.
- Separación de música, SFX, ambience y diálogo.
- Presupuesto de audio por plataforma.

## 6. Video y rendimiento

- HDR runtime probe.
- Resolución dinámica.
- Límite de FPS.
- Niveles de escalabilidad 0–4.
- Presupuestos de streaming.
- Métricas de memoria física.
- Protección adaptativa ante presión de memoria.
- Objetivo principal de 60 Hz donde el hardware/perfil lo permita.
- Perfil dedicado para hardware de menor capacidad.

Estado de ingeniería: los controles existen; el presupuesto definitivo requiere capturas de hardware real.

## 7. Lifecycle y continuidad

- SaveGame real de 10 slots.
- Guardado preventivo en deactivate/background.
- Recuperación foreground.
- Identidad activa rastreada por provider.
- Eventos de lifecycle.
- Marcadores de crash/hang.
- Recolección de evidencia de crash y build.

## 8. Accesibilidad y localización

- Subtítulos configurables.
- Escala de subtítulos.
- Alto contraste.
- Reducción de movimiento.
- Asistencia de color.
- Cultura nativa es.
- Matriz runtime de culturas.
- Target de localización Unreal.

Estado de contenido: las traducciones finales requieren producción y revisión lingüística.

## 9. Contenido actualizable

- Catálogo data-driven con IDs únicos.
- Catálogo de achievements.
- Verificación de entitlements.
- Lectura de title content/update files.
- Packaging fail-closed.
- El sistema no genera parches ficticios ni falsifica publicaciones de store.

## 10. QA y CI

- Validación automática de arquitectura y catálogos.
- Validación de contratos de plataforma.
- Validación de datos de gameplay/economía.
- Validación de jobs de packaging.
- Tests de integración del API cloud.
- Build UE5.6 real preparado para runner dedicado.
- Jobs manuales separados por plataforma.
- Generación de provenance verificable para cada pipeline.

## 11. Release Candidate

Un Release Candidate (RC) es una build candidata a validación final, no una declaración de certificación.

El gate RC exige:

1. CI de código y datos en verde;
2. build UE5.6 real para la plataforma objetivo;
3. contenido final auditado y sin referencias rotas;
4. tests de gameplay, saves, red, memoria, streaming y lifecycle;
5. provenance + manifest de artefacto;
6. crash/hang evidence;
7. firma externa aplicada fuera del repositorio;
8. matriz de hardware ejecutada.

La ausencia de SDK/hardware o de assets finales mantiene el RC en estado bloqueado.

## 12. Certificación

Los gates internos no equivalen a certificación del fabricante.

La certificación final requiere:

1. SDK autorizado;
2. hardware objetivo;
3. build cocinado/empaquetado real;
4. pruebas de lifecycle, saves, input, online, achievements, entitlements, audio, video, memoria y streaming;
5. localización y accesibilidad;
6. crash/hang validation;
7. requisitos vigentes del fabricante;
8. submission oficial.

## Regla de integridad

Nunca se considerará una capacidad “certificada” porque exista un archivo, una interfaz o un job. La evidencia debe provenir de ejecución real del producto en el entorno que corresponda.
