# Omnis — Multiplayer Architecture

## Objective

Support persistent continents, player-built settlements, territorial control and tactical sieges without compromising authoritative game state.

## Authority

The server owns combat outcomes, inventory/economy state, progression, territory and persistence-critical mutations.

## Replication

Replication is reserved for state that clients must observe. Cosmetic and presentation-only information should not become authoritative gameplay state.

## Persistence

Persistent world state will be separated from transient session state and written through explicit services with idempotent mutation semantics.
