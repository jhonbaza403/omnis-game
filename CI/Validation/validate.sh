#!/usr/bin/env bash
set -euo pipefail
python3 Tools/Validation/validate_omnis.py
python3 Tools/Data/validate_catalog.py
python3 Tools/World/validate_world_manifest.py
