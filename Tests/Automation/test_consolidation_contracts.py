import ast
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def first_statement_call(source, function_name):
    tree = ast.parse(source)
    function = next(
        node for node in tree.body
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef))
        and node.name == function_name
    )
    statement = function.body[0]
    if not isinstance(statement, ast.Expr) or not isinstance(statement.value, ast.Call):
        raise AssertionError(f"{function_name} must start with a guard call")
    return statement.value.func


class ConsolidationContractsTests(unittest.TestCase):
    def test_windows_build_has_one_implementation(self):
        canonical = (ROOT / "Tools/Build/build_omnis_windows.ps1").read_text(encoding="utf-8")
        compatibility = (ROOT / "Tools/Build/compile_omnis.ps1").read_text(encoding="utf-8")

        self.assertIn("Build.bat", canonical)
        self.assertIn("OmnisEditor", canonical)
        self.assertIn("Omnis", canonical)
        self.assertIn("build_omnis_windows.ps1", compatibility)
        self.assertNotIn("RunUBT", compatibility)
        self.assertNotIn("Build.bat", compatibility)

    def test_world_materializers_guard_before_mutation(self):
        base_path = ROOT / "Tools/Editor/GenerateOmnisWorldContent.py"
        premium_path = ROOT / "Tools/Editor/MaterializeOmnisPremiumWorld.py"
        base_source = base_path.read_text(encoding="utf-8")
        premium_source = premium_path.read_text(encoding="utf-8")

        base_call = first_statement_call(base_source, "main")
        premium_call = first_statement_call(premium_source, "main")
        self.assertIsInstance(base_call, ast.Name)
        self.assertEqual(base_call.id, "ensure_map_absent")
        self.assertIsInstance(premium_call, ast.Attribute)
        self.assertEqual(premium_call.attr, "ensure_map_absent")
        self.assertIn("Paths.file_exists", base_source)
        self.assertIn("does_asset_exist", base_source)

    def test_native_automation_gate_matches_a_real_test(self):
        test_source = (
            ROOT / "Source/Omnis/Developer/Automation/OmnisExperienceAutomationTests.cpp"
        ).read_text(encoding="utf-8")
        runner_source = (ROOT / "CI/Tests/run_unreal_automation.sh").read_text(encoding="utf-8")

        self.assertIn('"Omnis.Experience.Systems"', test_source)
        self.assertIn("Automation RunTests Omnis", runner_source)
        self.assertIn("Omnis.Experience.Systems", runner_source)

    def test_native_workflow_contains_the_release_gates(self):
        workflow = (
            ROOT / ".github/workflows/omnis-ue58-native-build.yml"
        ).read_text(encoding="utf-8")
        required_steps = (
            "build_omnis.sh",
            "generate_content.sh",
            "run_unreal_automation.sh",
            "package_omnis.sh",
            "smoke_client.sh",
            "package_omnis_server.sh",
            "smoke_dedicated_server.sh",
        )
        for step in required_steps:
            with self.subTest(step=step):
                self.assertIn(step, workflow)

    def test_packaging_refuses_to_replace_prior_outputs(self):
        client = (ROOT / "Tools/Build/package_omnis.sh").read_text(encoding="utf-8")
        server = (ROOT / "Tools/Build/package_omnis_server.sh").read_text(encoding="utf-8")

        self.assertIn("Refusing to overwrite an existing client package", client)
        self.assertIn("Refusing to overwrite an existing server package", server)
        self.assertNotIn("rm -rf", client)
        self.assertNotIn("rm -rf", server)


if __name__ == "__main__":
    unittest.main()
