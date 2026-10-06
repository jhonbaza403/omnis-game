# Omnis — Data Architecture

## Data-driven content

Definitions for items, characters, factions, quests, cycles and abilities should be represented as explicit data assets or equivalent Unreal data structures.

## Separation

Definitions describe what an object is. Runtime instances describe its current state. Save data contains only the persistent subset required to restore a valid experience.

## Validation

Content validation tools should detect duplicate identifiers, invalid references and incompatible data before packaging.
