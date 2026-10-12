# Unity 6.3 LTS workspace and GitLab runner contract

## Workspace

The canonical Unity project is in `UnityProject/`. The intended Editor version is Unity 6.3 LTS `6000.3.0f1`, pinned in `ProjectSettings/ProjectVersion.txt` and CI's `UNITY_VERSION`. Use the matching Editor and Linux Build Support module for reproducible builds.

## Required runner

A repository commit cannot install or register a runner on a physical host. A project/group administrator must register and maintain a Linux Shell runner with this tag:

- `omnis-unity63`

Install Unity Editor `6000.3.0f1` on that host and make the Editor executable available as `unity-editor`, or set the CI/CD variable `UNITY_EDITOR_PATH` to its absolute path. The runner must execute shell jobs and access the project checkout. Do not commit runner authentication tokens.

## CI/CD variables

Configure `UNITY_LICENSE` as a protected File variable containing a valid Unity license file. Protect the default branch and ensure the runner can run protected jobs. Never paste license contents into issues or commits.

Lightweight validation, Pages and Node/Cloudflare jobs use the project-owned Docker runner tagged `omnis-linux-docker`. Unity Editor import, PlayMode and Linux build jobs use a separate Linux Shell runner tagged `omnis-unity63`. The optional Windows build remains isolated on a Windows runner tagged `omnis-windows`. If one physical host is used for multiple roles, register distinct runner configurations with the appropriate executor and tag.

## Acceptance gates

1. Static project validation passes.
2. Unity Editor imports the project without compile errors.
3. PlayMode tests pass and publish results.
4. Linux player build succeeds and the artifact launches.
5. A manual gameplay smoke test verifies movement, camera, interactions, combat, enemy defeat, relic/Nexo progression, save/load and HUD.
6. Performance is measured on target hardware; 4K is a target, not a guarantee from configuration alone.

Do not label Omnis as compiled or playable until Editor import, PlayMode, build and launch have real evidence.
