import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

class ConsoleReadinessTests(unittest.TestCase):
    def test_enhanced_input_configuration(self):
        config = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
        self.assertIn("EnhancedPlayerInput", config)
        self.assertIn("EnhancedInputComponent", config)

    def test_gamepad_bindings_are_declared(self):
        data = json.loads((ROOT / "Content/Omnis/Input/input-map.json").read_text(encoding="utf-8"))
        self.assertEqual(data["system"], "EnhancedInput")
        self.assertFalse(data["policy"]["legacyActionMappings"])
        self.assertFalse(data["policy"]["legacyAxisMappings"])
        gamepad_keys = "
".join(
            binding
            for action in data["actions"].values()
            for binding in action.get("gamepad", [])
        )
        for key in ("Gamepad_LeftY", "Gamepad_LeftX", "Gamepad_RightX", "Gamepad_RightY"):
            self.assertIn(key, gamepad_keys)

    def test_accessibility_controls_exist(self):
        header = (ROOT / "Source/Omnis/UI/Accessibility/OmnisAccessibilitySubsystem.h").read_text(encoding="utf-8")
        for symbol in ("SetSubtitleScale", "SetSubtitleMode", "SetHighContrastUI", "SetReduceMotion", "SetColorBlindAssist"):
            self.assertIn(symbol, header)

    def test_release_gates_are_documented(self):
        docs = (ROOT / "Docs/PLAYSTATION_CERTIFICATION_READINESS.md").read_text(encoding="utf-8")
        self.assertIn("P4 — Submission", docs)
        self.assertIn("60 Hz", docs)
        self.assertIn("hardware final", docs)

if __name__ == "__main__":
    unittest.main()
