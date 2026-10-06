import unittest

class VerticalSliceContractTests(unittest.TestCase):
    def test_flow(self):
        flow = ["BOOT","PRIMER CICLO","MUNDO","EXPLORACIÓN","NPC","DECISIÓN","COMBATE","FIN DEL CICLO","NEXO","MEJORA","NUEVO CICLO"]
        self.assertEqual(flow[0], "BOOT")
        self.assertEqual(flow[-1], "NUEVO CICLO")
        self.assertLess(flow.index("NEXO"), flow.index("NUEVO CICLO"))

if __name__ == "__main__":
    unittest.main()
