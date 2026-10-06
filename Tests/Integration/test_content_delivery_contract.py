import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


class ContentDeliveryContractTests(unittest.TestCase):
    def test_contract_forbids_fake_evidence(self):
        data = json.loads(
            (ROOT / "Content/Omnis/Data/content-delivery-contract.json")
            .read_text(encoding="utf-8")
        )
        self.assertTrue(data["noFakeEvidence"])

    def test_every_release_gate_requires_explicit_evidence(self):
        data = json.loads(
            (ROOT / "Content/Omnis/Data/content-delivery-contract.json")
            .read_text(encoding="utf-8")
        )
        for spec in data["deliverables"].values():
            self.assertTrue(spec["requiredPaths"])
            self.assertTrue(spec["requiredExtensions"])
            self.assertTrue(spec["qaEvidencePath"])

        for spec in data["hardwareEvidence"].values():
            self.assertTrue(spec["evidencePath"])

        for spec in data["platformIntegration"].values():
            self.assertTrue(spec["evidencePath"])

        for spec in data["storeAndCertification"].values():
            self.assertTrue(spec["evidencePath"])


if __name__ == "__main__":
    unittest.main()
