#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BALANCE_PATH = ROOT / "Content/Omnis/Data/balance.json"
ENCOUNTERS_PATH = ROOT / "Content/Omnis/Data/encounters.json"
NARRATIVE_PATH = ROOT / "Content/Omnis/Data/narrative.json"

def fail(message: str) -> None:
    raise AssertionError(message)

def load(path: Path):
    if not path.is_file():
        fail(f"missing data file: {path.relative_to(ROOT)}")
    return json.loads(path.read_text(encoding="utf-8"))

def main() -> None:
    balance = load(BALANCE_PATH)
    encounters = load(ENCOUNTERS_PATH)
    narrative = load(NARRATIVE_PATH)

    player = balance["player"]
    if not (0 < player["maxHealth"] <= 1000):
        fail("player.maxHealth outside production bounds")
    if not (0 < player["walkSpeed"] < player["sprintSpeed"] <= 2000):
        fail("player movement speeds are invalid")
    if not (0 < balance["combat"]["lightAttack"]["damage"] <= 1000):
        fail("combat damage outside production bounds")
    if not (0 < balance["combat"]["lightAttack"]["cooldownSeconds"] <= 10):
        fail("combat cooldown outside production bounds")
    if balance["construction"]["resourceCost"][0]["quantity"] <= 0:
        fail("construction resource cost must be positive")
    if balance["progression"]["experiencePerLevel"] <= 0:
        fail("progression experiencePerLevel must be positive")
    if balance["performanceTargets"]["targetHz"] < 30:
        fail("target Hz is invalid")
    if balance["performanceTargets"]["frameBudgetMs"] > (1000.0 / balance["performanceTargets"]["targetHz"]) + 0.01:
        fail("frame budget exceeds target refresh budget")

    encounter_ids = [entry["id"] for entry in encounters["encounters"]]
    if len(encounter_ids) != len(set(encounter_ids)):
        fail("encounter ids must be unique")
    if len(encounter_ids) < 3:
        fail("vertical slice requires at least three encounter definitions")

    beats = narrative["beats"]
    if len(beats) < 5:
        fail("vertical slice narrative requires at least five authored beats")
    beat_ids = [entry["id"] for entry in beats]
    if len(beat_ids) != len(set(beat_ids)):
        fail("narrative beat ids must be unique")
    if any(len(entry["text"].strip()) < 10 for entry in beats):
        fail("narrative beat text is too short")

    print("[PASS] data-driven balance, encounters and narrative validated.")

if __name__ == "__main__":
    main()
