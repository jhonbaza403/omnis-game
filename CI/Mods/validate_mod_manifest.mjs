import assert from "node:assert/strict";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const here = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(here, "../..");
const modsRoot = path.join(root, "Content/Omnis/Mods/Samples");
const manifestPath = path.join(modsRoot, "manifest.json");
const worldPackPath = path.join(root, "Content/Omnis/Data/venezuela-triangle-v1.json");

const manifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));
const worldPack = JSON.parse(fs.readFileSync(worldPackPath, "utf8"));

assert.equal(manifest.schema_version, 1);
assert.ok(/^[a-z0-9]+([.-][a-z0-9]+)+$/.test(manifest.package_id));
assert.match(manifest.version, /^\d+\.\d+\.\d+$/);
assert.equal(manifest.engine_target, "Unreal Engine 5.8");
assert.equal(manifest.target_world_pack, worldPack.world_pack.id);
assert.equal(manifest.package_status, "unsigned_authoring_template_not_installable");
assert.equal(manifest.signature_policy.required_for_production, true);
assert.equal(manifest.signature_policy.algorithm, "Ed25519");
assert.equal(manifest.signature_policy.status, "unsigned_authoring_template");
assert.equal(manifest.signature_policy.signature, null);

for (const permission of [
  "execute_scripts",
  "execute_native_code",
  "filesystem_access",
  "external_network_access"
]) {
  assert.equal(manifest.permissions[permission], false, permission + " must remain disabled");
}

const allowedTypes = new Set([
  "world_event_rules",
  "mission_data",
  "dialogue_data",
  "economy_data",
  "vehicle_tuning",
  "interior_theme"
]);
for (const contentType of manifest.content_types) {
  assert.ok(allowedTypes.has(contentType), "Unsupported mod content type: " + contentType);
}

const routes = new Set(worldPack.routes.map((route) => route.id));
const markets = new Map(worldPack.markets.map((market) => [market.id, new Set(market.commodities)]));
const contentPaths = new Set();

for (const contentFile of manifest.content_files) {
  assert.ok(allowedTypes.has(contentFile.type), "Unsupported file type: " + contentFile.type);
  assert.ok(typeof contentFile.path === "string" && contentFile.path.length > 0);
  assert.ok(!path.isAbsolute(contentFile.path), "Absolute package paths are forbidden");
  const normalized = path.posix.normalize(contentFile.path.replaceAll("\\", "/"));
  assert.ok(!normalized.startsWith("../") && normalized !== "..", "Path traversal is forbidden");
  assert.ok(normalized.endsWith(".json"), "Only JSON data files are allowed in this starter contract");
  assert.ok(!contentPaths.has(normalized), "Duplicate package content path: " + normalized);
  contentPaths.add(normalized);

  const absolute = path.resolve(modsRoot, normalized);
  assert.ok(absolute.startsWith(modsRoot + path.sep), "Content path escapes the sample package");
  const data = JSON.parse(fs.readFileSync(absolute, "utf8"));

  if (contentFile.type === "world_event_rules") {
    assert.equal(data.package_id, manifest.package_id);
    const eventIds = new Set();
    for (const event of data.events) {
      assert.ok(event.id && !eventIds.has(event.id), "Missing or duplicate event ID");
      eventIds.add(event.id);
      for (const routeId of event.route_ids) {
        assert.ok(routes.has(routeId), event.id + " references unknown route " + routeId);
      }
      for (const effect of event.effects) {
        assert.ok(markets.has(effect.market_id), event.id + " references unknown market " + effect.market_id);
        assert.ok(markets.get(effect.market_id).has(effect.item_id),
          event.id + " references an item not sold by " + effect.market_id);
        assert.ok(Number.isInteger(effect.supply_delta), "Supply changes must be integer values");
        assert.ok(Number.isInteger(effect.demand_delta), "Demand changes must be integer values");
      }
    }
  }
}

assert.ok(contentPaths.size > 0, "A package must contain at least one declared data file");
console.log(
  "Validated data-only community package template: " + manifest.package_id +
  ", " + contentPaths.size + " content files. It remains unsigned and non-installable."
);
