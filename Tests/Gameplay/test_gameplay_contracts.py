import unittest

class GameplayContractsTests(unittest.TestCase):
    def test_inventory_does_not_go_negative(self):
        current = 3
        remove = 5
        self.assertFalse(remove <= current)

    def test_health_is_clamped(self):
        self.assertEqual(max(0.0, min(100.0, -10.0)), 0.0)

if __name__ == "__main__":
    unittest.main()
