# Omnis — Release Engineering

Este documento define la infraestructura de release que puede quedar auditada dentro del repositorio sin fabricar evidencia externa.

## Provenance

Cada pipeline puede generar Saved/Release/provenance.json con:

- commit, rama y pipeline de GitLab;
- versión de Unreal declarada en Omnis.uproject;
- módulos y plugins;
- inventario de archivos y SHA-256;
- SHA-256 del artefacto final cuando exista.

El manifest es evidencia de procedencia del código y del artefacto; no sustituye la firma de distribución.

## Crash evidence

Tools/Diagnostics/collect_crash_evidence.py recopila hashes de dumps/logs presentes en Saved/Crashes y Saved/Logs. El repositorio no inventa dumps ni resultados de crash-free rate.

La operación profesional de producción debe añadir:

- symbol server;
- bucketing de crashes;
- minidump/core dump processing;
- correlación con versión;
- dashboards de crash-free sessions y hang rate.

## Firma

Las claves de firma permanecen fuera del repositorio. El pipeline de RC debe recibir el material de firma por secretos protegidos del proveedor CI y fallar cerrado cuando no exista.

## SBOM/provenance

El provenance de origen enumera archivos y componentes de Unreal declarados. No se presenta como un SBOM completo de terceros: los SDK propietarios, toolchains y dependencias externas deben incorporarse en el proceso de release autorizado.

## Release Candidate

Tools/Release/validate_release_candidate.py solo admite una RC cuando:

- existe una build real;
- el SDK autorizado está presente para consolas;
- el manifest de readiness está en READY;
- provenance y crash evidence están generados.

Mientras Content/Omnis/Data/release-readiness.json esté en BLOCKED, una RC real debe permanecer bloqueada.
