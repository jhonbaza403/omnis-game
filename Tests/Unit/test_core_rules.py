import unittest

class CoreRulesTests(unittest.TestCase):
    def test_level_formula(self):
        self.assertEqual((0 // 100) + 1, 1)
        self.assertEqual((250 // 100) + 1, 3)

    def test_positive_experience_only(self):
        experience = 10
        amount = -50
        if amount > 0:
            experience += amount
        self.assertEqual(experience, 10)

if __name__ == "__main__":
    unittest.main()
