# Unity 7 Alpha workspace and GitLab runner contract

## Experimental engine pin

The canonical Unity project is in `UnityProject/`. This branch intentionally targets Unity 7.0 Alpha `7000.0.0a8`, pinned in `ProjectSettings/ProjectVersion.txt` and CI's `UNITY_VERSION`.

Unity identifies this as an Alpha release and recommends backing up projects before opening them in an Alpha Editor. Keep this migration on the feature branch. Do not merge it to `main` until package resolution, Editor import, PlayMode tests, builds, and launch checks pass. Unity 6 LTS remains the safer production baseline.

Official release page: https://unity.com/releases/editor/alpha
Official Unity 7 manual: https://docs.unity.com/en-us/engine/7000.0/manual

## Required runner

A repository commit cannot install or register a runner on a physical host. A project/group administrator must install the matching Unity 7 Alpha Editor and register a Linux Shell runner with this tag:

- `omnis-unity7`

Install Editor `7000.0.0a8` and Linux Build Support on that host. Make the Editor executable available as `unity-editor`, or set CI/CD variable `UNITY_EDITOR_PATH` to its absolute path. Do not commit runner authentication tokens.

## CI/CD variables

Configure `UNITY_LICENSE` as a protected File variable containing a valid Unity license file. Protect the default branch and ensure the runner can run protected jobs. Never paste license contents into issues or commits.

Lightweight validation, Pages and Node/Cloudflare jobs use `omnis-linux-docker`. Unity Editor import, PlayMode and Linux build jobs use the dedicated `omnis-unity7` runner. The optional Windows build remains isolated on `omnis-windows` and must have Editor `7000.0.0a8` plus Windows Build Support installed.

## Compatibility and acceptance gates

1. Static project validation passes.
2. Unity 7 Alpha Editor imports the project without compile errors.
3. Package Manager resolves the existing URP, Input System and Test Framework dependencies; package compatibility must be confirmed by the Editor, not assumed.
4. PlayMode tests pass and publish results.
5. Linux player build succeeds and the artifact launches.
6. Manual gameplay smoke test verifies movement, camera, interactions, combat, enemy defeat, relic/Nexo progression, save/load and HUD.
7. Performance is measured on target hardware; 4K is a target, not a guarantee from configuration alone.

Do not label Omnis as compiled or playable until Editor import, PlayMode, build and launch have real evidence. Do not merge to `main` while the Unity 7 Alpha runner is missing or any gate is pending or failing.
