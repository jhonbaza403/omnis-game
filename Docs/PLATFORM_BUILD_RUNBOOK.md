# Omnis — Platform Build Runbook

## Real build contract

The repository uses one gameplay codebase and one Unreal project. Platform packaging is selected by build environment, not by branching gameplay.

### Supported packaging profiles

| Profile | UAT platform | Runner requirement |
| --- | --- | --- |
| Win64 | Win64 | UE 5.6 runner |
| PlayStation 5 | PS5 | Authorized console runner + platform SDK |
| Xbox Series | XSX | Authorized console runner + platform GDK/SDK |
| Nintendo target | Switch | Authorized console runner + platform SDK |

The exact vendor SDK version is pinned by the authorized build environment. Credentials and platform binaries remain outside source control.

## Required Release Candidate evidence

Each platform package must retain:

- commit SHA and UE 5.6 Build.version;
- cook/stage/package logs;
- automation test results;
- performance and memory captures;
- save/suspend/resume evidence;
- online/session/entitlement/achievement evidence;
- localization and accessibility matrix;
- crash/hang results;
- platform submission validation output.

A repository CI pass is not a platform certification result.

## Fail-closed behavior

Tools/Build/package_platform.sh exits when:

- UE 5.6 is not installed;
- RunUAT is absent;
- the platform is unsupported;
- a proprietary console build is requested without an explicitly authorized runner marker.

No fake SDK, fake package, or fake certification result is generated.
