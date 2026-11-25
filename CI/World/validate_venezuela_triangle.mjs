import assert from "node:assert/strict";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const here = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(here, "../..");
const catalogPath = path.join(root, "Content/Omnis/Data/venezuela-triangle-v1.json");
const economyCppPath = path.join(root, "Source/Omnis/World/Economy/OmnisEconomySubsystem.cpp");
const economyHeaderPath = path.join(root, "Source/Omnis/World/Economy/OmnisEconomySubsystem.h");
const damageHeaderPath = path.join(root, "Source/Omnis/World/Destruction/OmnisStructuralDamageComponent.h");
const damageSubsystemPath = path.join(root, "Source/Omnis/World/Destruction/OmnisStructuralDamageSubsystem.cpp");
const saveHeaderPath = path.join(root, "Source/Omnis/Core/Save/OmnisSaveGame.h");
const persistencePath = path.join(root, "Source/Omnis/Multiplayer/Persistence/OmnisPersistenceSubsystem.cpp");
const questDefinitionPath = path.join(root, "Source/Omnis/Data/Quests/OmnisQuestDefinition.h");
const questSubsystemPath = path.join(root, "Source/Omnis/Narrative/Quests/OmnisQuestSubsystem.cpp");

const catalog = JSON.parse(fs.readFileSync(catalogPath, "utf8"));
const economyCpp = fs.readFileSync(economyCppPath, "utf8");
const economyHeader = fs.readFileSync(economyHeaderPath, "utf8");
const damageHeader = fs.readFileSync(damageHeaderPath, "utf8");
const damageSubsystem = fs.readFileSync(damageSubsystemPath, "utf8");
const saveHeader = fs.readFileSync(saveHeaderPath, "utf8");
const persistence = fs.readFileSync(persistencePath, "utf8");
const questDefinition = fs.readFileSync(questDefinitionPath, "utf8");
const questSubsystem = fs.readFileSync(questSubsystemPath, "utf8");

assert.equal(catalog.schema_version, 1);
assert.equal(catalog.catalog_id, "omnis.worldpack.venezuela-triangle.v1");
assert.equal(catalog.status, "design_catalog_not_a_materialized_playable_map");
assert.equal(catalog.engine_target, "Unreal Engine 5.8");
assert.equal(catalog.world_pack.id, "WorldPack.ModernVenezuela");
assert.equal(catalog.world_pack.interior_access_target, 0.8);
assert.equal(catalog.world_pack.interior_access_target_status, "production_target_not_measured");

const regionIds = new Set();
const subregionIds = new Set();
for (const region of catalog.regions) {
  assert.ok(region.id && region.name, "Every region needs a stable ID and name");
  assert.ok(!regionIds.has(region.id), "Duplicate region ID: " + region.id);
  regionIds.add(region.id);
  for (const subregion of region.subregions) {
    assert.ok(subregion.id && subregion.name, "Every subregion needs a stable ID and name");
    assert.ok(!subregionIds.has(subregion.id), "Duplicate subregion ID: " + subregion.id);
    subregionIds.add(subregion.id);
    assert.equal(subregion.anchor.length, 3, subregion.id + " needs a 3D game-space anchor");
    assert.ok(subregion.anchor.every(Number.isFinite), subregion.id + " has an invalid anchor");
  }
}
assert.deepEqual(
  [...regionIds].sort(),
  ["Region.Barlovento", "Region.Caracas", "Region.GuarenasGuatire", "Region.LaGuaira"].sort()
);

const routeIds = new Set();
const marketIds = new Set(catalog.markets.map((market) => market.id));
assert.equal(marketIds.size, catalog.markets.length, "Market IDs must be unique");

for (const route of catalog.routes) {
  assert.ok(route.id && !routeIds.has(route.id), "Missing or duplicate route ID: " + route.id);
  routeIds.add(route.id);
  for (const nodeId of [route.from, ...route.via, route.to]) {
    assert.ok(subregionIds.has(nodeId), route.id + " references unknown subregion " + nodeId);
  }
  for (const marketId of route.logistics_links) {
    assert.ok(marketIds.has(marketId), route.id + " references unknown market " + marketId);
  }
}

const phaseOneNodes = new Set(
  catalog.routes.filter((route) => route.phase === 1)
    .flatMap((route) => [route.from, ...route.via, route.to])
);
for (const required of ["Caracas.East", "Guarenas", "Guatire", "Caucagua", "RioChico", "Higuerote"]) {
  assert.ok(phaseOneNodes.has(required), "Phase 1 is missing " + required);
}
assert.ok(catalog.routes.filter((route) => route.phase === 1).length >= 2);
assert.ok(catalog.routes.some((route) => route.id.includes("TunnelHighway") && route.phase === 2));
assert.ok(catalog.routes.some((route) => route.id.includes("OldRoad") && route.phase === 2));
assert.ok(catalog.routes.some((route) => route.id.includes("SeaLane") && route.phase === 3));

for (const market of catalog.markets) {
  assert.ok(regionIds.has(market.region), market.id + " references unknown region " + market.region);
  assert.ok(economyCpp.includes('TEXT("' + market.region + '")'), market.id + " runtime seed uses a different region ID");
  assert.ok(market.commodities.length > 0, market.id + " has no commodities");
  assert.ok(economyCpp.includes('TEXT("' + market.id + '")'), market.id + " is missing from the runtime seed");
  for (const itemId of market.commodities) {
    assert.ok(economyCpp.includes('TEXT("' + itemId + '")'), itemId + " is missing from the runtime seed");
  }
}
assert.ok(catalog.markets.some((market) => market.visibility === "restricted"));
assert.ok(economyCpp.includes("bPubliclyVisible"));
assert.ok(economyCpp.includes("Market.Caracas.Black"));
assert.ok(economyCpp.includes("Seed.Phase > SafeThroughPhase"));
assert.ok(economyCpp.includes("SafeThroughPhase = FMath::Clamp(ThroughPhase, 1, 3)"));
assert.ok(economyCpp.includes("Currency.USD"), "Port and restricted markets need their configured currency IDs");
assert.ok(economyHeader.includes("ExecuteBarter"));
assert.ok(economyHeader.includes("ExecuteTrade"));
assert.ok(economyCpp.includes("HasMutationAuthority"));
assert.ok(economyCpp.includes("OfferedMarket->CurrencyId != RequestedMarket->CurrencyId"));
assert.ok(economyCpp.includes("ApplyRouteSupplyShock"));
assert.ok(economyCpp.includes("Route.Caucagua-RioChico-Higuerote"));
assert.ok(economyCpp.includes("Market.Higuerote.Coastal"));
assert.ok(questDefinition.includes("FailureAffectedRouteIds"));
assert.ok(questDefinition.includes("RecoveryAffectedRouteIds"));
assert.ok(questSubsystem.includes("Economy->ApplyRouteSupplyShock"));
assert.ok(damageHeader.includes("ApplyStructuralDamage"));
assert.ok(damageHeader.includes("RepairStructuralPart"));
assert.ok(damageHeader.includes("PersistentStructureId"));
assert.ok(damageSubsystem.includes("StructureSnapshots.Add"));
assert.ok(damageSubsystem.includes("RegisterStructure(Component)"));
assert.ok(saveHeader.includes("SchemaVersion = 7"));
assert.ok(saveHeader.includes("StructuralDamageSnapshots"));
assert.ok(saveHeader.includes("TerrainDeformationSnapshots"));
assert.ok(saveHeader.includes("FactionTerritoryStates"));
assert.ok(saveHeader.includes("FOmnisSavedVehicleState"));
assert.ok(persistence.includes("Save->SchemaVersion != 6"));
assert.ok(persistence.includes("CurrentSaveSchemaVersion = 7"));
assert.ok(persistence.includes("Damage->ImportSnapshots(Save->StructuralDamageSnapshots)"));

assert.equal(catalog.production_phases.map((phase) => phase.phase).join(","), "1,2,3");
assert.equal(catalog.systemic_targets.creator_ecosystem.arbitrary_native_code_execution, "not_allowed");

console.log(
  "Validated Venezuela Triangle design catalog: " + catalog.regions.length + " regions, " +
  subregionIds.size + " subregions, " + catalog.routes.length + " routes, " +
  catalog.markets.length + " markets. Catalog remains design-only until UE maps/assets are materialized."
);
