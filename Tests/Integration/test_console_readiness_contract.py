from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]

class ConsoleReadinessTests(unittest.TestCase):
    def test_controller_inputs_exist(self):
        config = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
        for key in ("Gamepad_LeftY", "Gamepad_LeftX", "Gamepad_RightX", "Gamepad_RightY"):
            self.assertIn(key, config)

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
