# Unity 7 workspace and GitLab runner contract

## Workspace

The canonical playable project lives in `UnityProject/`. The requested Unity 7 preview is pinned to `7000.0.0a7` in `ProjectSettings/ProjectVersion.txt` and CI's `UNITY_VERSION`. Unity 7 is currently an Alpha; keep this as a development target, not a production-release claim. Back up the project before opening it in the Alpha Editor.

## Required runner

A repository commit cannot install a runner on a physical host. A project/group administrator must register and maintain a Linux Shell runner with this tag:

- `omnis-unity7`

Install Unity Editor `7000.0.0a7` on that host, include Linux Build Support, and make the editor executable available as `unity-editor` or set the CI/CD variable `UNITY_EDITOR_PATH` to its absolute path. The runner must be able to run shell jobs and access the project checkout. Do not place runner authentication tokens in Git.

## CI/CD variables

Configure `UNITY_LICENSE` as a protected File variable containing a valid Unity license file. Protect the default branch and ensure the runner is permitted to run protected jobs. Do not paste license content into issues or commits.

To avoid shared-runner quota exhaustion, lightweight validation, Pages, and Node/Cloudflare jobs use the project-owned Docker runner tagged `omnis-linux-docker`. That runner must be active and configured with a Docker executor. Jobs that launch the Unity Editor use a separate Linux Shell runner tagged `omnis-unity7`; it needs Unity 7 Alpha installed locally. The Windows build remains isolated on a Windows runner tagged `omnis-windows`. If one physical host is used for more than one role, register distinct runner configurations with the appropriate executor and tag; do not assume one runner with one tag can satisfy all jobs.

## Acceptance gates

1. Pipeline source validation passes.
2. The Unity Editor job imports the project without compile errors.
3. PlayMode tests pass and publish test results.
4. Linux player build succeeds and the build artifact is downloaded and launched.
5. A manual gameplay smoke test verifies keyboard/controller movement, camera, interactions, combat, enemy defeat, relic/Nexo progression, save/load, and HUD.
6. Visual performance is checked on the actual target hardware; 4K is a target, not a guarantee from configuration alone.

Never label Omnis as compiled/playable until gates 2–5 have real evidence.
