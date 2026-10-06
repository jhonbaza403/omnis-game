# Omnis — Content Pipeline

## Source control

Text source and Unreal configuration belong in Git. Binary assets are tracked through Git LFS.

## Authoring flow

Design definition → implementation → content authoring → validation → packaging.

## Quality gate

A content change is complete when references resolve, runtime behavior is validated and generated build artifacts remain excluded from source control.
