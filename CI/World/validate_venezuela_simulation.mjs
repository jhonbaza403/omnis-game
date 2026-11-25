import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";
const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "../..");
const read = (relative) => JSON.parse(fs.readFileSync(path.join(root, relative), "utf8"));
const fail = (message) => { throw new Error(message); };
const unique = (values, label) => { const seen = new Set(); for (const value of values) { if (!value || seen.has(value)) fail(label + ": empty or duplicate id " + value); seen.add(value); } };
const terrain = read("Content/Omnis/Data/venezuela-terrain-profile-v1.json");
const factions = read("Content/Omnis/Data/venezuela-factions-v1.json");
const vehicles = read("Content/Omnis/Data/vehicle-physics-v1.json");
const mission = read("Content/Omnis/Data/Missions/operation-cargo-echo-v1.json");
if ([terrain, factions, vehicles, mission].some(x => x.schema_version !== 1)) fail("Unsupported catalog schema.");
unique(terrain.profiles.map(x => x.id), "terrain profile");
const byProfile = Object.fromEntries(terrain.profiles.map(x => [x.id, x]));
for (const profile of terrain.profiles) {
 if (!profile.samples || profile.samples.length < 2) fail(profile.id + ": at least two altitude samples required");
 let last = -1;
 for (const sample of profile.samples) {
  if (!Number.isFinite(sample.progress) || sample.progress < 0 || sample.progress > 1 || sample.progress <= last || !Number.isFinite(sample.altitude_m) || sample.altitude_m < -5 || sample.altitude_m > 3500) fail(profile.id + ": invalid or unordered altitude sample");
  last = sample.progress;
 }
 if (profile.samples[0].progress !== 0 || last !== 1) fail(profile.id + ": profile must cover [0,1]");
}
if (Math.max(...byProfile["Profile.WarairaRepano.Ridge"].samples.map(x => x.altitude_m)) < 2700) fail("Waraira Repano crest must reach approximately 2700 m.");
const coast = byProfile["Profile.Caracas-LaGuaira.TunnelHighway"].samples;
if (coast[0].altitude_m < 850 || coast.at(-1).altitude_m > 5) fail("Caracas-La Guaira profile must descend to sea level.");
const lowland = byProfile["Profile.CaracasEast-Barlovento"].samples.at(-1).altitude_m;
if (lowland < 0 || lowland > 50) fail("Barlovento terminal altitude must be within 0–50 m.");
unique(terrain.biomes.map(x => x.id), "biome"); unique(terrain.surfaces.map(x => x.id), "surface");
const surfaceIds = new Set(terrain.surfaces.map(x => x.id));
for (const biome of terrain.biomes) for (const surface of biome.surfaces) if (!surfaceIds.has(surface)) fail(biome.id + ": unknown surface " + surface);
for (const surface of terrain.surfaces) if (surface.base_grip <= 0 || surface.base_grip > 1 || surface.wet_grip_multiplier <= 0 || surface.wet_grip_multiplier > 1 || surface.rolling_resistance < 0 || surface.mud_deformation_rate < 0 || surface.corrosion_rate_per_hour < 0) fail(surface.id + ": surface values outside supported bounds");
for (const gate of terrain.reveal_gates) if (!gate.region_id || !Array.isArray(gate.events) || gate.events.length === 0) fail("Every reveal gate needs a region and an event.");
unique(factions.factions.map(x => x.id), "faction"); unique(factions.territories.map(x => x.id), "territory");
const factionIds = new Set(factions.factions.map(x => x.id));
for (const territory of factions.territories) if (!factionIds.has(territory.dominant_faction) || territory.access_standing_threshold < -100 || territory.access_standing_threshold > 100) fail(territory.id + ": invalid faction reference or threshold");
const response = factions.response_thresholds;
if (!(response.patrol_from < response.mobile_units_from && response.mobile_units_from < response.coordinated_response_from && response.coordinated_response_from <= 100)) fail("Security response thresholds must be ordered.");
unique(vehicles.vehicles.map(x => x.id), "vehicle profile");
for (const vehicle of vehicles.vehicles) if (vehicle.fuel_capacity_l <= 0 || vehicle.base_consumption_l_per_hour <= 0 || !["motorcycle", "sedan", "4x4", "boat"].includes(vehicle.category)) fail(vehicle.id + ": invalid vehicle profile");
for (const category of ["motorcycle", "sedan", "4x4", "boat"]) if (!vehicles.vehicles.some(x => x.category === category)) fail("Missing vehicle category " + category);
if (vehicles.vehicles.filter(x => x.category === "boat").length < 2) fail("Two boat profiles are required.");
unique(mission.phases.map(x => x.id), "mission step");
const stepIds = new Set(mission.phases.map(x => x.id));
for (const step of mission.phases) { for (const next of step.next || []) if (!stepIds.has(next)) fail(step.id + ": next step does not exist: " + next); for (const branch of step.branches || []) if (!stepIds.has(branch.next)) fail(step.id + ": branch target does not exist: " + branch.next); }
if (!mission.phases.some(x => x.terminal === true)) fail("Mission needs a terminal step.");
console.log("Validated " + terrain.profiles.length + " altitude profiles, " + terrain.biomes.length + " biomes, " + terrain.surfaces.length + " surfaces, " + factions.factions.length + " factions, " + factions.territories.length + " territories, " + vehicles.vehicles.length + " vehicle profiles and " + mission.phases.length + " mission steps.");
console.log("Design data validated; this does not certify Landscape, World Partition, Chaos rigs or mission assets.");
