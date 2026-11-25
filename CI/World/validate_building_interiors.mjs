import assert from "node:assert/strict";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const here = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(here, "../..");
const interiorsPath = path.join(root, "Content/Omnis/Data/building-interiors-v1.json");
const worldPath = path.join(root, "Content/Omnis/Data/venezuela-triangle-v1.json");
const interiors = JSON.parse(fs.readFileSync(interiorsPath, "utf8"));
const world = JSON.parse(fs.readFileSync(worldPath, "utf8"));

assert.equal(interiors.schema_version, 1);
assert.equal(interiors.catalog_id, "omnis.building-interiors.v1");
assert.equal(interiors.status, "design_inventory_not_materialized_or_measured");
assert.equal(interiors.engine_target, "Unreal Engine 5.8");
assert.equal(interiors.target_metric.target_ratio, 0.8);
assert.equal(interiors.target_metric.planned_building_inventory_count, interiors.buildings.length);
assert.equal(interiors.target_metric.measured_accessible_buildings, 0);
assert.equal(interiors.target_metric.measured_total_buildings, 0);
assert.equal(interiors.target_metric.measured_ratio, null);
assert.equal(interiors.target_metric.measurement_status, "not_measured_no_native_maps_or_building_assets_verified");

const subregions = new Set(world.regions.flatMap((region) => region.subregions.map((subregion) => subregion.id)));
const persistentIds = new Set();
const interiorIds = new Set();
let phaseOneCount = 0;

for (const building of interiors.buildings) {
  assert.ok(building.persistent_id && !persistentIds.has(building.persistent_id),
    "Missing or duplicate building PersistentId: " + building.persistent_id);
  persistentIds.add(building.persistent_id);
  assert.ok(building.interior_id && !interiorIds.has(building.interior_id),
    "Missing or duplicate interior ID: " + building.interior_id);
  interiorIds.add(building.interior_id);
  assert.ok(subregions.has(building.subregion_id),
    building.persistent_id + " references unknown subregion " + building.subregion_id);
  assert.ok(building.entry_id, building.persistent_id + " needs an entry ID");
  assert.equal(building.entry_anchor_relative.length, 3);
  assert.ok(building.entry_anchor_relative.every(Number.isFinite));
  assert.ok(Number.isInteger(building.floors) && building.floors >= 1);
  assert.ok(building.room_templates.length > 0);
  assert.ok(building.vertical_routes.length > 0);
  assert.equal(building.accessible_interior_target, true);
  assert.equal(building.materialization_status, "not_materialized");
  assert.equal(building.accessibility_test_status, "not_tested");
  if (building.phase === 1) phaseOneCount += 1;
}

const plannedTarget = Math.ceil(interiors.buildings.length * interiors.target_metric.target_ratio);
assert.equal(interiors.target_metric.planned_accessible_building_target, plannedTarget);
assert.equal(interiors.target_metric.phase_1_building_count, phaseOneCount);
assert.equal(
  interiors.target_metric.phase_1_accessible_target,
  Math.ceil(phaseOneCount * interiors.target_metric.target_ratio)
);

console.log(
  "Validated building/interior access registry: " + interiors.buildings.length +
  " planned buildings; target " + plannedTarget + " accessible. No interiors are yet counted as materialized or tested."
);
