#!/usr/bin/env bash
set -euo pipefail
python3 Tools/Validation/validate_omnis.py
python3 Tools/Data/validate_catalog.py
python3 Tools/World/validate_world_manifest.py
python3 Tools/Validation/validate_console_readiness.py
python3 Tools/Validation/validate_platform_contracts.py
