from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]
WORKER = (ROOT / "Cloudflare/worker.js").read_text(encoding="utf-8")
WRANGLER = (ROOT / "wrangler.json").read_text(encoding="utf-8")

class CloudApiContractTests(unittest.TestCase):
    def test_worker_exposes_real_game_api_routes(self):
        for route in (
            'url.pathname === "/v1/profile"',
            'url.pathname === "/v1/progression"',
            'url.pathname === "/v1/events"',
        ):
            self.assertIn(route, WORKER)
        # Real saves route family: /v1/saves/
        self.assertIn("/v1/saves/", WORKER.replace("\\/", "/"))

    def test_worker_requires_bearer_auth_for_api(self):
        self.assertIn("authorization", WORKER)
        self.assertIn("authentication_required", WORKER)
        self.assertIn("/auth/v1/user", WORKER)

    def test_worker_targets_dedicated_omnis_supabase(self):
        self.assertIn("mqdrcgymylbqozarykip.supabase.co", WRANGLER)

    def test_no_service_role_secret_in_worker(self):
        self.assertNotIn("service_role", WORKER)

if __name__ == "__main__":
    unittest.main()
