# Omnis — Gameplay Architecture

Gameplay is separated into reusable rules and player-facing composition.

## Ownership

Core subsystems own cross-world state. Gameplay actors own local behavior. World systems own environmental state. UI observes state rather than becoming its authority.

## Authority

Single-player may execute locally, but architecture is written so future authoritative multiplayer rules do not depend on trusting a client-provided result.

## Extension

Abilities, inventory, equipment, combat and interaction are introduced as concrete systems when the vertical slice requires them.
