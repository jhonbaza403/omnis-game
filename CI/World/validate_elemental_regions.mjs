import { readFileSync } from "node:fs";
import assert from "node:assert/strict";

const file = "Content/Omnis/Data/elemental-regions-v1.json";
const catalog = JSON.parse(readFileSync(file, "utf8"));

assert.equal(catalog.schema_version, 1, "Unexpected catalog schema version");
assert.equal(catalog.world_plan.region_count, 12, "Expected exactly 12 regions");
assert.equal(catalog.regions.length, 12, "Catalog must define 12 region records");

const ids = catalog.regions.map((region) => region.id);
assert.equal(new Set(ids).size, 12, "Region IDs must be unique");

for (const region of catalog.regions) {
  assert.ok(region.name, `Region ${region.id} is missing a name`);
  assert.ok(region.element, `Region ${region.id} is missing its element`);
  assert.ok(region.core_mechanic, `Region ${region.id} is missing its core mechanic`);
  assert.ok(region.boss, `Region ${region.id} is missing its boss`);
}

const tierTotal = catalog.world_plan.subregion_tiers.reduce((sum, tier) => {
  assert.ok(tier.from <= tier.to, `Invalid range for tier ${tier.id}`);
  assert.equal(tier.count, tier.to - tier.from + 1, `Tier ${tier.id} count does not match its range`);
  return sum + tier.count;
}, 0);

assert.equal(tierTotal, catalog.world_plan.subregions_per_region, "Tier counts must equal 134");
assert.equal(
  catalog.world_plan.region_count * catalog.world_plan.subregions_per_region,
  catalog.world_plan.planned_subregion_total,
  "Planned total must equal region count multiplied by subregions per region",
);
assert.equal(catalog.world_plan.planned_subregion_total, 1608, "Expected 1,608 planned subregions");

console.log(`PASS: ${catalog.regions.length} regions; ${tierTotal} subregions per region; ${catalog.world_plan.planned_subregion_total} planned total.`);
