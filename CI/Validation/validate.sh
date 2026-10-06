#!/usr/bin/env bash
set -euo pipefail
python3 -m compileall -q Tools Tests
python3 Tools/Validation/validate_omnis.py
python3 Tools/Data/validate_catalog.py
python3 Tools/World/validate_world_manifest.py
python3 Tools/Validation/validate_console_readiness.py
python3 Tools/Validation/validate_platform_contracts.py
python3 Tools/Validation/validate_platform_build_contract.py
python3 Tools/Validation/validate_ci_platform_jobs.py
python3 Tools/Validation/validate_localization_contract.py
python3 Tools/Balance/validate_balance.py
python3 Tools/Release/audit_content_delivery.py
