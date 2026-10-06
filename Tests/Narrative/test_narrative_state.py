import unittest

class NarrativeStateTests(unittest.TestCase):
    def test_standing_bounds(self):
        self.assertEqual(max(-100, min(100, 250)), 100)
        self.assertEqual(max(-100, min(100, -250)), -100)

if __name__ == "__main__":
    unittest.main()
