# Omnis Unity migration status

## Verified starting point

The existing `main` branch is an Unreal Engine project: it contains `Omnis.uproject`, Unreal C++ modules, Unreal plugins, and Unreal content directories. The existing Cloudflare Worker is separate and remains at the repository root.

## What this commit does

- Adds a Unity 6.3 LTS project shell under `UnityProject/`.
- Adds a CI structure check for the Unity project shell.
- Adds a guarded Linux build entry point that refuses to build without a real migrated main scene.
- Separates Cloudflare validation, manual preview deployment, and manual production deployment.
- Preserves the existing GitLab Pages portal job.

## Not yet migrated or verified

- Unreal C++ gameplay and plugin systems have not been ported to C#.
- Unreal `.uasset` / `.umap` content has not been converted into Unity assets/scenes.
- No Unity gameplay scene is supplied by this scaffold. In particular, `UnityProject/Assets/Scenes/OmnisMain.unity` is intentionally absent.
- No Unity build has been claimed or verified by this commit.
- Cloudflare production deployment is not claimed by this commit.

## Unity migration rules

1. Import original source assets only when their origin and format are verified.
2. Recreate Unreal-only materials, blueprints, maps, animation graphs, and plugin behavior using Unity-native equivalents; do not rename Unreal binary files to pretend they are Unity assets.
3. Port and test gameplay systems in small, reviewable slices.
4. Create and commit a real main scene only after the first playable slice exists.
5. Configure `UNITY_LICENSE` as a masked/protected GitLab CI variable before running the Unity build job.
6. Configure `CLOUDFLARE_ACCOUNT_ID` and `CLOUDFLARE_API_TOKEN` as masked/protected GitLab CI variables before running Cloudflare preview/deployment jobs.

## Runner quota

A CI YAML change does not replenish GitLab shared-runner compute minutes. If the shared quota remains exhausted, use an available self-managed runner or restore the account/group quota before expecting jobs to execute.
