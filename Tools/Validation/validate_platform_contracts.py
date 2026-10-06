#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def require_text(path: str, fragments: list[str]) -> None:
    content = (ROOT / path).read_text(encoding="utf-8")
    missing = [fragment for fragment in fragments if fragment not in content]
    if missing:
        raise AssertionError(f"{path}: missing {missing}")

def main() -> None:
    require_text(
        "Source/Omnis/Platform/OmnisPlatformSubsystem.h",
        [
            "UnlockAchievement",
            "QueryAchievements",
            "ApplyHDR",
            "ApplyDynamicResolution",
            "ApplyFrameRateLimit",
            "ApplyOverallScalability",
            "SaveForLifecycleTransition",
            "GetAvailablePhysicalMemoryBytes",
        ],
    )
    require_text(
        "Source/Omnis/Online/Entitlements/OmnisContentEntitlementSubsystem.h",
        ["RefreshEntitlements", "IsContentEntitled"],
    )
    require_text(
        "Source/Omnis/Platform/OmnisPlatformSubsystem.cpp",
        [
            "ApplicationWillDeactivateDelegate",
            "ApplicationWillEnterBackgroundDelegate",
            "ApplicationHasEnteredForegroundDelegate",
            "OnHandleSystemError",
            "OnHandleSystemHang",
            "FPlatformMemory::GetStats",
        ],
    )
    require_text(
        "Config/OmnisPlatformProfiles.ini",
        ["[PlayStation5]", "[XboxSeries]", "[Switch2]", "AchievementsProvider=OnlineSubsystem"],
    )
    require_text(
        "Config/Localization/OmnisGame.ini",
        ["NativeCulture=es", "CulturesToGenerate=en", "CulturesToGenerate=ja", "CulturesToGenerate=zh-Hans"],
    )
    require_text(
        "Docs/PLAYSTATION_CERTIFICATION_READINESS.md",
        ["P0", "P1", "P2", "P3", "P4"],
    )
    print("[PASS] unified platform, content, lifecycle and localization contracts validated.")

if __name__ == "__main__":
    main()
