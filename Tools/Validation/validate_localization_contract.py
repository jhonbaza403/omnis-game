#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def main() -> None:
    game = (ROOT / "Config/DefaultGame.ini").read_text(encoding="utf-8")
    loc = (ROOT / "Config/Localization/OmnisGame.ini").read_text(encoding="utf-8")
    for text in ("[Omnis.Localization]", "NativeCulture=es", "SupportedCultures="):
        if text not in game:
            raise AssertionError(f"DefaultGame.ini missing {text}")
    for text in ("CulturesToGenerate=en", "CulturesToGenerate=ja", "CulturesToGenerate=zh-Hans"):
        if text not in loc:
            raise AssertionError(f"OmnisGame.ini missing {text}")
    print("[PASS] runtime localization contract validated.")

if __name__ == "__main__":
    main()
