# Omnis — Docker Multiplayer Architecture

## Principle

Omnis separates the **authoritative gameplay plane** from the **server infrastructure plane**. The Unreal dedicated server owns replicated gameplay truth. Docker runs the control-plane microservices that allocate sessions, queue players, and persist the meta-state of Los Ciclos.

## Containers

| Container | Responsibility | State |
|---|---|---|
| gateway | HTTP ingress and routing | stateless |
| matchmaking | Playlist queue and match claims | Redis |
| session | Session allocation and 24-hour lease metadata | Redis |
| world | Cycle/era revision and narrative world events | Redis AOF |
| redis | Shared durable queue/session/world metadata | named volume |
| omnis-dedicated-server | Real UE 5.8 headless gameplay server | packaged UE artifact |

## Omnis world model

The first persistent era is **Ruinas de Piedra Ciénaga**. The world service stores \`cycle_index\`, \`world_revision\`, the current era, and an append-only event sequence. A gameplay death or world transition can call \`POST /v1/world/cycle/advance\`; narrative decisions can be recorded through \`POST /v1/world/events\`.

This preserves the intended fiction: death returns the player to the Nexo while the shared world continues to advance. The microservices store the timeline anchor; they do not simulate NPC routines, physics, combat, or rendering.

## API flow

1. Client -> gateway -> matchmaking to enqueue a player.
2. A trusted orchestrator or server-side worker claims a pair from matchmaking.
3. session records the session and the current \`world_revision\`.
4. The client connects to the Unreal dedicated server over UDP.
5. Unreal remains authoritative for player/world gameplay replication.
6. Server-side events can update world so the persistent timeline survives individual deaths and sessions.

## Security boundary

The gateway requires \`X-Omnis-Player-Id\` for player-facing matchmaking/session calls and for write operations on the world API. This is an integration boundary, not a replacement for the project's eventual real identity/JWT layer. Internal calls use \`X-Omnis-Service-Token\` when configured.

Only the gateway HTTP port and the dedicated-server UDP port are published. Redis is on the internal network only.

## Dedicated-server image

\`infra/docker/unreal-server/Dockerfile\` copies only \`Build/OmnisServer/\`. The image deliberately fails when that directory does not exist. No fake executable, fake \`.uasset\`, or fake \`.umap\` is generated.

The dedicated server target and package flow are designed around a real UE 5.8 server build.

## CI/CD

GitLab's Container Registry is used for SHA-tagged microservice and dedicated-server images. The pipeline never requires a committed production secret and does not overwrite \`latest\`.

Compose healthchecks and \`depends_on: condition: service_healthy\` are used so dependent services start only after Redis/backend readiness.
