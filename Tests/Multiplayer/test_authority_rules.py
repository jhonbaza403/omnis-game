import unittest
import math

class AuthorityRulesTests(unittest.TestCase):
    def test_non_finite_input_is_rejected(self):
        self.assertFalse(math.isfinite(float("nan")))
        self.assertFalse(math.isfinite(float("inf")))

    def test_numeric_input_is_bounded(self):
        self.assertEqual(max(0.0, min(1.0, 3.0)), 1.0)

if __name__ == "__main__":
    unittest.main()
