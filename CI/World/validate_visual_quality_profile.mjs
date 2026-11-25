import { readFileSync } from "node:fs";
import assert from "node:assert/strict";

const file = "Content/Omnis/Data/visual-quality-profile.json";
const profile = JSON.parse(readFileSync(file, "utf8"));

assert.equal(profile.schema_version, 1);
assert.equal(profile.rendering.engine, "Unreal Engine 5.8");
assert.equal(profile.rendering.target_resolution.width, 3840);
assert.equal(profile.rendering.target_resolution.height, 2160);
assert.ok(profile.visual_direction.style.includes("photorealistic"));
assert.ok(profile.visual_direction.avoid.includes("comic-book rendering"));
assert.ok(profile.characters.animation.length >= 4);
assert.ok(profile.nature.systems.includes("species-appropriate vegetation"));
assert.ok(profile.interface.requirements.some((item) => item.includes("3840x2160")));
assert.ok(profile.interface.requirements.some((item) => item.includes("1280x720")));
assert.ok(profile.quality_gates.asset_status_values.includes("runtime-tested"));
assert.ok(profile.quality_gates.asset_status_values.includes("performance-verified"));
assert.ok(profile.rendering.performance_targets.high_end_pc.includes("not a universal guarantee"));
assert.ok(profile.quality_gates.first_vertical_slice.length >= 5);

console.log("PASS: photoreal direction, 4K target, scalability, characters, nature, responsive UI and evidence gates.");
