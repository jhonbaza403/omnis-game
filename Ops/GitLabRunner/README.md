# Omnis — GitLab self-hosted runner UE 5.8

Este proyecto usa un runner propio de GitLab para que el tramo Unreal y el empaquetado no dependan de la cuota de compute de runners SaaS.

## Requisitos
Host Linux x86_64 con Docker Engine.

Recomendado para el pipeline completo: 8 CPU, 24 GB RAM y al menos 80 GB libres; 120+ GB es preferible.

Imagen Unreal: wukakuki/unreal-engine:dev-slim-5.8.3@sha256:3f0886cf0d7e5bc9cea0e7ba27f417e0d0febb5ad0c371c9a89223c6112c80ef

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

## Runner actual — UE 5.8.3

La imagen de GitLab Runner está fijada a `gitlab/gitlab-runner:alpine-v19.4.1`. El executor es Docker y el runner debe tener acceso al Docker Engine mediante `/var/run/docker.sock`.

Requisitos del host para el pipeline Unreal:

- Linux x86_64.
- 8 CPU o más.
- 24 GB RAM o más.
- 80 GB libres como mínimo; 120 GB recomendados.
- Docker Engine instalado.
- Conectividad de salida a `gitlab.com`, Docker Hub y el Container Registry.

El runner se registra con el token de autenticación de proyecto `glrt-...` que GitLab entrega al crear el project runner. GitLab documenta el flujo actual basado en runner authentication tokens; los registration tokens antiguos están obsoletos. citeturn823219search0turn823219search1
