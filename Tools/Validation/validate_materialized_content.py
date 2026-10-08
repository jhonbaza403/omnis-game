#!/usr/bin/env python3
"""Verify that Unreal Engine produced real native map/material assets."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
MAP=ROOT/"Content/Omnis/Maps/Omnis_OpenWorld.umap"
ART=ROOT/"Content/Omnis/Art"
MATS=ROOT/"Content/Omnis/Materials"
if not MAP.is_file() or MAP.stat().st_size==0:
    raise SystemExit("FAIL: native Omnis_OpenWorld.umap is missing or empty")
native=sorted(ART.rglob("*.uasset")) if ART.exists() else []
materials=sorted(MATS.rglob("*.uasset")) if MATS.exists() else []
if len(native)<63:
    raise SystemExit(f"FAIL: expected at least 63 native Art .uasset files, found {len(native)}")
if len(materials)<10:
    raise SystemExit(f"FAIL: expected at least 10 material .uasset files, found {len(materials)}")
print("=== REAL UNREAL MATERIALIZATION VERIFIED ===")
print(f"Map: {MAP} ({MAP.stat().st_size} bytes)")
print(f"Native Art assets: {len(native)}")
print(f"Material assets: {len(materials)}")
