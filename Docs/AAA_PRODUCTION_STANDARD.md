# Omnis — Estándar de producción AAA

Omnis no se considera terminado por cantidad de clases o directorios. Se considera listo por resultados medibles de producto.

## 1. Dirección visual
- Identidad artística propia.
- Art bible.
- Materiales PBR coherentes.
- Iluminación y composición consistentes.
- Personajes con rigs y deformación de calidad.
- Animación locomotion/combat/cinematic producida y revisada.
- VFX y audio sincronizados con gameplay.

## 2. Mundo
- World Partition y streaming medidos.
- Diseño de niveles por experiencia.
- Densidad de interacción controlada.
- NPC schedules y population budgets.
- Herramientas internas de world building.
- Validación de memoria por región.

## 3. Gameplay
- Input responsive.
- Cámara consistente.
- Combate legible y equilibrado.
- Traversal fiable.
- Construcción con reglas deterministas.
- Inventario y progresión persistentes.
- Economía y balance medidos.
- Loop de ciclo completo.

## 4. Narrativa
- Biblia narrativa.
- Arcos de personajes.
- Diálogo versionado.
- Localización preparada desde el inicio.
- Sistema de decisiones y consecuencias determinista.
- Cinematics integradas con gameplay.
- Herramientas de revisión y QA narrativa.

## 5. Online
- Identidad segura.
- Persistencia autoritativa.
- Recuperación de red.
- Rate limits.
- Observabilidad.
- Telemetría minimizada y protegida.
- Pruebas de carga reales.
- Dedicated server cuando la arquitectura final lo exija.

## 6. Calidad de consola
- Mando.
- Safe area.
- Accesibilidad.
- Subtítulos.
- Reanudación.
- Guardado.
- Errores recuperables.
- No depender de teclado/ratón.
- Rendimiento medido sobre hardware objetivo.

## 7. Operación
- CI reproducible.
- Builds trazables.
- Versionado.
- Crash reporting.
- Symbol management.
- Release channels.
- Rollback.
- Backups.
- Incidentes documentados.

## 8. Puerta de salida

No se declara Release Candidate hasta que:
1. no existan blockers críticos;
2. el vertical slice completo pueda jugarse de principio a fin;
3. el rendimiento cumpla su presupuesto;
4. los saves sobrevivan a recuperación;
5. el sistema de entrada funcione con mando;
6. la accesibilidad crítica esté validada;
7. las pruebas de red y larga duración pasen;
8. la build de plataforma objetivo haya sido ejecutada en hardware real.
