# Unity 7 Alpha compatibility and runner contract

## Engine pin

This branch targets Unity 7.0 Alpha `7000.0.0a8`. Alpha releases are experimental; back up the project and do not treat an Editor-version pin as proof that the project compiles.

Official references:
- https://unity.com/releases/editor/alpha
- https://docs.unity.com/en-us/engine/7000.0/manual/render-pipelines/universal-render-pipeline/requirements

## URP migration

The previous manifest pinned URP `17.3.0`, associated with Unity 6.3. It has been replaced by URP `18.0.0` as the Unity 7 generation package candidate. **This is a migration change, not a verified compile result.** The exact Alpha editor must resolve this package and its transitive SRP packages; if Package Manager rejects it, use the URP version bundled/recommended by that exact Editor installation rather than reverting to the obsolete Unity 6.3 pin.

Unity's documentation states that each Alpha/Beta/patch Editor ships with an up-to-date core graphics package. The Unity Editor import gate must confirm the actual resolved package version. Input System `1.20.1` and Test Framework `1.4.6` also remain subject to Editor package-resolution and compilation checks.

## Required runner and license

A repository commit cannot register a physical GitLab runner. A project/group administrator must register a runner tagged `omnis-unity7`, install Unity `7000.0.0a8` with Linux Build Support, and configure `UNITY_EDITOR_PATH` if the executable is not `unity-editor`. Configure `UNITY_LICENSE` as a protected File CI/CD variable containing a valid license file. Do not commit license data or runner tokens.

Lightweight checks use `omnis-linux-docker`. Editor import, PlayMode tests, and the Linux player build require `omnis-unity7`.

## Required acceptance gates

1. Static structure validation passes.
2. Unity 7 Alpha Package Manager resolves URP, Input System, Test Framework and dependencies.
3. Editor import completes without C# compilation errors.
4. PlayMode tests pass and publish a JUnit report.
5. Linux player build succeeds and the artifact launches.
6. A gameplay smoke test verifies movement, camera, interaction, combat, progression, save/load and HUD.
7. Performance is measured on target hardware; 4K settings are a target, not a guarantee.

The Linux build is configured as a merge-request gate now, not only a default-branch build. Do not merge to `main` until all Editor, tests, build and launch gates are green. No compatibility or playability claim is valid until the Unity runner actually executes them.
