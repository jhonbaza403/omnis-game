#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def require_text(path: str, fragments: list[str]) -> None:
    content = (ROOT / path).read_text(encoding="utf-8")
    missing = [fragment for fragment in fragments if fragment not in content]
    if missing:
        raise AssertionError(f"{path}: missing {missing}")

def main() -> None:
    require_text("Source/Omnis/Omnis.Build.cs", ["OnlineSubsystem", "EnhancedInput"])
    require_text(
        "Omnis.uproject",
        ['"Name": "OnlineSubsystem", "Enabled": true', '"Name": "EnhancedInput", "Enabled": true'],
    )
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
        "Source/Omnis/Online/Entitlements/OmnisContentEntitlementSubsystem.h",
        ["RefreshEntitlements", "IsContentEntitled"],
    )
    require_text(
        "Source/Omnis/Online/Content/OmnisTitleUpdateSubsystem.h",
        ["EnumerateUpdateFiles", "ReadUpdateFile", "GetUpdateFileContents"],
    )
    require_text(
        "Config/DefaultInput.ini",
        ["EnhancedPlayerInput", "EnhancedInputComponent"],
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
        "Docs/PLATFORM_RUNTIME_STANDARD.md",
        ["IOnlineAchievements", "IOnlineEntitlements", "UGameUserSettings", "P0–P4"],
    )
    print("[PASS] unified console platform contracts validated.")

if __name__ == "__main__":
    main()
