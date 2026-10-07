# Omnis — GitLab self-hosted runner UE 5.6

Este proyecto usa un runner propio de GitLab para que el tramo Unreal y el empaquetado no dependan de la cuota de compute de runners SaaS.

## Requisitos
Host Linux x86_64 con Docker Engine.

Recomendado para el pipeline completo: 8 CPU, 24 GB RAM y al menos 80 GB libres; 120+ GB es preferible.

Imagen Unreal: wukakuki/unreal-engine:dev-slim-5.6.1@sha256:7ff40023124f2e2773fc43ad904f3b1305c1157c7b58328e0c08937edeb425d9

## Crear el runner
GitLab → Settings → CI/CD → Runners → Create project runner.
Etiqueta obligatoria: omnis-ci-linux
Desactiva Run untagged.

## Registrar
export RUNNER_AUTHENTICATION_TOKEN='glrt-...'
bash Ops/GitLabRunner/register.sh

## Verificar
bash Ops/GitLabRunner/verify.sh

## Flujo
Runner preflight → Python validation → Python tests → Cloudflare dry-run → Unreal preflight → build C++ → materialización UAsset/UMAP → Automation Tests → dedicated server → smoke → imagen Docker → smoke del contenedor.

Los .uasset y .umap se generan exclusivamente dentro de Unreal Engine.