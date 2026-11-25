import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def load(relative):
    return json.loads((ROOT / relative).read_text(encoding="utf-8"))


class VenezuelaSimulationCatalogTests(unittest.TestCase):
    def test_altitude_profiles_cover_caracas_coast_and_barlovento(self):
        profiles = {
            x["id"]: x
            for x in load("Content/Omnis/Data/venezuela-terrain-profile-v1.json")["profiles"]
        }
        self.assertGreaterEqual(
            max(x["altitude_m"] for x in profiles["Profile.WarairaRepano.Ridge"]["samples"]),
            2700,
        )
        coast = profiles["Profile.Caracas-LaGuaira.TunnelHighway"]["samples"]
        self.assertGreaterEqual(coast[0]["altitude_m"], 850)
        self.assertLessEqual(coast[-1]["altitude_m"], 5)
        self.assertLessEqual(
            profiles["Profile.CaracasEast-Barlovento"]["samples"][-1]["altitude_m"],
            50,
        )

    def test_biome_surface_references_and_coastal_reveal_gate(self):
        catalog = load("Content/Omnis/Data/venezuela-terrain-profile-v1.json")
        surfaces = {x["id"] for x in catalog["surfaces"]}
        for biome in catalog["biomes"]:
            self.assertTrue(set(biome["surfaces"]).issubset(surfaces))
        gates = {x["region_id"]: x["events"] for x in catalog["reveal_gates"]}
        self.assertIn(
            "WorldGate.CaracasLaGuaira.TunnelExit",
            gates["Region.LaGuaira"],
        )

    def test_faction_territories_resolve(self):
        catalog = load("Content/Omnis/Data/venezuela-factions-v1.json")
        ids = {x["id"] for x in catalog["factions"]}
        self.assertEqual(len(catalog["territories"]), 4)
        for territory in catalog["territories"]:
            self.assertIn(territory["dominant_faction"], ids)

    def test_vehicle_catalog_covers_land_and_water(self):
        categories = [
            x["category"]
            for x in load("Content/Omnis/Data/vehicle-physics-v1.json")["vehicles"]
        ]
        for category in ("motorcycle", "sedan", "4x4", "boat"):
            self.assertIn(category, categories)
        self.assertGreaterEqual(categories.count("boat"), 2)

    def test_altitude_grade_query_feeds_vehicle_brake_and_engine_heat(self):
        terrain_h = (
            ROOT / "Source/Omnis/World/Terrain/OmnisVenezuelaTerrainSubsystem.h"
        ).read_text(encoding="utf-8")
        terrain_cpp = (
            ROOT / "Source/Omnis/World/Terrain/OmnisVenezuelaTerrainSubsystem.cpp"
        ).read_text(encoding="utf-8")
        vehicle_h = (
            ROOT / "Source/Omnis/Gameplay/Vehicle/OmnisVehicleConditionComponent.h"
        ).read_text(encoding="utf-8")
        vehicle_cpp = (
            ROOT / "Source/Omnis/Gameplay/Vehicle/OmnisVehicleConditionComponent.cpp"
        ).read_text(encoding="utf-8")
        pawn_cpp = (
            ROOT / "Source/Omnis/Gameplay/Vehicle/OmnisVehiclePawn.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("GetGradePercent", terrain_h)
        self.assertIn("EndAltitudeMeters - StartAltitudeMeters", terrain_cpp)
        self.assertIn("SetRoadGradePercent", vehicle_h)
        self.assertIn("DownhillBrakeLoad", vehicle_cpp)
        self.assertIn("UphillLoadFactor", vehicle_cpp)
        self.assertIn(
            "VehicleCondition->SetRoadGradePercent(RoadGradePercent)",
            pawn_cpp,
        )

    def test_mission_graph_and_persistence_targets(self):
        mission = load(
            "Content/Omnis/Data/Missions/operation-cargo-echo-v1.json"
        )
        ids = {x["id"] for x in mission["phases"]}
        for step in mission["phases"]:
            for target in step.get("next", []):
                self.assertIn(target, ids)
            for branch in step.get("branches", []):
                self.assertIn(branch["next"], ids)
        self.assertTrue(any(x.get("terminal") for x in mission["phases"]))
        self.assertIn(
            "vehicle_condition_snapshots",
            mission["phases"][-1]["save_fields"],
        )

    def test_region_streaming_is_backed_by_world_partition_providers(self):
        streaming_h = (
            ROOT / "Source/Omnis/World/Streaming/OmnisStreamingSubsystem.h"
        ).read_text(encoding="utf-8")
        streaming_cpp = (
            ROOT / "Source/Omnis/World/Streaming/OmnisStreamingSubsystem.cpp"
        ).read_text(encoding="utf-8")
        self.assertIn("IsRegionStreamingComplete", streaming_h)
        self.assertIn("UWorldPartitionStreamingSourceComponent", streaming_cpp)
        self.assertIn("GetSubsystem<UWorldPartitionSubsystem>", streaming_cpp)
        self.assertIn("EnableStreamingSource", streaming_cpp)
        self.assertIn("DisableStreamingSource", streaming_cpp)
        self.assertIn("IsStreamingSourceProviderRegistered", streaming_cpp)
        self.assertIn("IsStreamingCompleted", streaming_cpp)
        self.assertNotIn("LoadedRegions", streaming_cpp)

        catalog = load("Content/Omnis/Data/venezuela-triangle-v1.json")
        self.assertGreaterEqual(len(catalog["regions"]), 4)
        for region in catalog["regions"]:
            self.assertTrue(region.get("subregions"))
            for subregion in region["subregions"]:
                self.assertEqual(len(subregion.get("anchor", [])), 3)


if __name__ == "__main__":
    unittest.main()
