# Omnis — Save System

## Scope

The save layer records player and cycle state that must survive process restarts.

## Rules

Save operations should be explicit, versionable and resilient to interrupted writes. Runtime transient state must not be serialized just because it exists in memory.

## Future versioning

Save schema changes require deliberate version migration. No fabricated migrations are introduced until an actual schema change exists.
