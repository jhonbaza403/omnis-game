#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def main() -> None:
    balance = json.loads((ROOT / "Content/Omnis/Data/balance.json").read_text(encoding="utf-8"))
    encounters = json.loads((ROOT / "Content/Omnis/Data/encounters.json").read_text(encoding="utf-8"))
    base_damage = balance["combat"]["lightAttack"]["damage"]

    rows = []
    for mode, profile in (("Standard", balance["difficulty"]["Standard"]), ("Hard", balance["difficulty"]["Hard"])):
        effective_enemy_health = 100.0 * profile["enemyHealthMultiplier"]
        attacks_to_defeat = int((effective_enemy_health + base_damage - 1) // base_damage)
        rows.append({
            "difficulty":mode,
            "effectiveEnemyHealth":effective_enemy_health,
            "playerAttackDamage":base_damage,
            "attacksToDefeat100HPEnemy":attacks_to_defeat,
            "frameBudgetMs":balance["performanceTargets"]["frameBudgetMs"]
        })

    print(json.dumps({
        "encounters":[entry["id"] for entry in encounters["encounters"]],
        "difficultySimulation":rows,
        "status":"design_validation_only",
        "note":"Tuning validation does not substitute for gameplay or hardware profiling."
    }, indent=2, ensure_ascii=False))

if __name__ == "__main__":
    main()
