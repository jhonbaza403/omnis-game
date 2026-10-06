# Omnis: El Último Ciclo — Vertical Slice Real

Este documento define qué puede quedar realmente jugable con la infraestructura actual, sin inventar binarios, servicios ficticios ni assets inexistentes.

## Núcleo jugable

El proyecto contiene movimiento, exploración, interacción, construcción, combate, IA, narrativa, mundo, UI, online y subsistemas de plataforma. La base de persistencia permite que estos sistemas compartan un estado verificable.

## Persistencia

### Local
Diez ranuras reales mediante SaveGame de Unreal Engine.

### Cloud
El cliente Unreal puede usar un access token de Supabase para:
- cargar el perfil;
- consultar progresión;
- guardar una ranura;
- cargar una ranura;
- eliminar una ranura;
- registrar eventos.

El Worker comprueba la identidad mediante Supabase Auth y no requiere service_role en el cliente.

## Ciclo de juego

BOOT → PRIMER CICLO → EXPLORACIÓN → INTERACCIÓN/NPC → DECISIÓN → RESULTADO → FIN DE CICLO → NEXO → PROGRESIÓN → NUEVO CICLO.

La capa de resultado está preparada para combinar decisiones, recursos, reputación, estado del mundo y consecuencias.

## Qué falta para un juego comercial completo

Código de base no equivale a contenido AAA. Todavía hacen falta assets 3D, animaciones, audio, UI final, diseño de niveles, guion, voz, iluminación, VFX, tuning, QA, optimización, builds para cada plataforma, infraestructura de servidores dedicada cuando corresponda y pruebas de capacidad reales.

## Regla de integridad

No se crean mocks que se presenten como producción. No se replica Credi. No se crean tablas espejo innecesarias. Las credenciales privilegiadas nunca forman parte del cliente.
