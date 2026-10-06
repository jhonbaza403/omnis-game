#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
result = subprocess.run(
    [sys.executable, "-m", "unittest", "discover", "-s", str(ROOT / "Tests"), "-p", "test_*.py"],
    cwd=ROOT,
)
raise SystemExit(result.returncode)
