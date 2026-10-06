import time
import unittest

class PerformanceContractTests(unittest.TestCase):
    def test_small_loop_budget(self):
        start = time.perf_counter()
        sum(range(10000))
        self.assertLess(time.perf_counter() - start, 0.1)

if __name__ == "__main__":
    unittest.main()
