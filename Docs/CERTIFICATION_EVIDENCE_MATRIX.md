# Omnis — Certification Evidence Matrix

This document separates implementation evidence from manufacturer certification.

| Gate | Repository evidence | Hardware evidence | External gate |
| --- | --- | --- | --- |
| Runtime lifecycle | Platform subsystem + save-on-background | Required | Vendor validation |
| User context | Online identity refresh + HUD display name | Required | Vendor validation |
| Saves | Local SaveGame + cloud persistence | Required | Vendor validation |
| Achievements | Data-driven catalog + IOnlineAchievements | Required | Achievement catalog publication |
| Entitlements | IOnlineEntitlements | Required | Store entitlement validation |
| Title content/update | IOnlineTitleFile | Required | Store/update pipeline |
| Online sessions | IOnlineSession | Required | Platform service test |
| Performance | UGameUserSettings + adaptive memory protection | Required | Device performance pass |
| Streaming | Async asset handles + streaming budgets | Required | World/level hardware profiling |
| Audio focus | Audio focus delegate integration | Required | Device audio test |
| Localization | Runtime culture service + Unreal localization target | Required | Language QA |
| Crash/hang | Local markers + authenticated event telemetry | Required | Platform crash/log validation |
| Accessibility | Existing accessibility subsystem | Required | Accessibility QA |
| Submission | BuildCookRun packaging contract + CI gates | Required | Official submission validation |

## Release Candidate rule

An internal green repository pipeline means only that source contracts and available automated tests pass. A Release Candidate requires a real UE 5.6 package executed on the target device with all required evidence attached.

## No false certification

The project must never create or store:

- fake SDKs;
- fabricated console binaries;
- simulated certification reports presented as official;
- fake store submissions;
- fabricated achievement publication results.

Vendor-specific requirements are implemented only through authorized SDK/provider integrations and validated with the official submission process.
