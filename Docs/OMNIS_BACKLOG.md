# Omnis — backlog priorizado y materialización

## P0 — Bloqueantes
- [ ] Confirmar la versión exacta de Unity 7 instalada en el runner y si los paquetes del proyecto la soportan; resolver incompatibilidades de manifiesto.
- [ ] Confirmar runners activos `omnis-linux-docker` y `omnis-unity7`; validar ejecutores y permisos.
- [ ] Configurar `UNITY_LICENSE` como variable File protegida; no guardar licencia ni tokens en Git.
- [ ] Ejecutar importación/compilación real, PlayMode y build Linux; conservar logs y artefactos.
- [ ] Ejecutar el binario y completar el smoke test; anotar SO, CPU/GPU, resolución y resultado.
- [ ] Integrar en `main` solo tras gates verdes y revisión de la MR.

## P1 — Vertical slice
- [ ] Verificar input, controlador, cámara, colisión, animación y reinicio.
- [ ] Implementar ataque ligero/pesado, bloqueo, esquiva, stagger, daño y telegráficos.
- [ ] Implementar enemigo básico, pesado y jefe de dos fases.
- [ ] Completar arena, ruta, acertijo, tres reliquias y punto Nexo.
- [ ] Conectar HUD y guardado/carga.
- [ ] Crear pruebas PlayMode que verifiquen el bucle completo y los errores de regresión.

## P2 — Contenido y pulido
- [ ] Sistema de misiones y diálogo con consecuencias.
- [ ] Streaming por región y límites de CPU/memoria para mundo reactivo.
- [ ] Perfiles gráficos y pruebas de rendimiento por hardware.
- [ ] Accesibilidad, mando, subtítulos, remapeo y opciones de cámara.
- [ ] Inventario de licencias y atribuciones de todos los assets externos.

## P3 — Expansión
- [ ] Cooperativo y competición solo después de estabilizar el juego individual.
- [ ] Herramientas de creación de regiones y validación de datos.
- [ ] Más jefes, biomas, historias y ciclos tras demostrar que el vertical slice se mantiene estable.
