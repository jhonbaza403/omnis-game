import fs from "node:fs";
import path from "node:path";

const root = process.cwd();
const errors = [];
const required = [
  "ProjectSettings/ProjectVersion.txt",
  "Packages/manifest.json",
  "Assets/Scenes/OmnisPrototype.unity",
  "Assets/Scripts/OmnisBootstrap.cs",
  "Assets/Scripts/OmnisPlayerController.cs",
  "Assets/Scripts/OmnisCollectible.cs",
  "Assets/Scripts/OmnisPortal.cs",
  "Assets/Scripts/OmnisPremiumWorld.cs"
];
for (const file of required) {
  if (!fs.existsSync(path.join(root, file))) errors.push("Missing required file: " + file);
}
const read = (file) => fs.existsSync(path.join(root, file)) ? fs.readFileSync(path.join(root, file), "utf8") : "";
const version = read("ProjectSettings/ProjectVersion.txt");
if (!version.includes("6000.3.0f1")) errors.push("Unity editor version does not match the repository's declared version.");
const manifest = read("Packages/manifest.json");
if (!manifest.includes("com.unity.inputsystem")) errors.push("Input System package is not declared.");
const player = read("Assets/Scripts/OmnisPlayerController.cs");
for (const [label, token] of [
  ["keyboard movement", "Keyboard.current"],
  ["gamepad movement", "Gamepad.current"],
  ["jump", "jumpHeight"],
  ["sprint", "sprintMultiplier"],
  ["fall recovery", "ResetToStart"]
]) if (!player.includes(token)) errors.push("Player controller missing " + label + ".");
const bootstrap = read("Assets/Scripts/OmnisBootstrap.cs");
for (const [label, token] of [
  ["collectible objective", "RegisterCollectible"],
  ["portal objective", "EnterNexus"],
  ["modern input reset", "Keyboard.current.rKey"],
  ["camera follow", "CameraFollow"]
]) if (!bootstrap.includes(token)) errors.push("Bootstrap missing " + label + ".");
const scene = read("Assets/Scenes/OmnisPrototype.unity");
if (!scene.includes("Omnis Runtime Bootstrap")) errors.push("Prototype scene does not contain the expected bootstrap object.");
const meta = read("Assets/Scripts/OmnisBootstrap.cs.meta");
const guid = meta.split(/\r?\n/).find(line => line.startsWith("guid:"))?.slice("guid:".length).trim();
if (!guid || !scene.includes(guid)) errors.push("Prototype scene bootstrap script GUID does not match its .meta file.");
const premium = read("Assets/Scripts/OmnisPremiumWorld.cs");
for (const [label, token] of [
  ["world generation", "BuildNature()"],
  ["landmarks", "BuildNexusPlaza()"],
  ["water", "BuildWater()"],
  ["lighting and fog", "RenderSettings.fog = true"]
]) if (!premium.includes(token)) errors.push("Premium world missing " + label + ".");
const sourceAssets = [
  "Assets/Art/ImportedFromUnreal/Architecture/Omnis_House.obj",
  "Assets/Art/ImportedFromUnreal/Characters/Omnis_Civilian.obj",
  "Assets/Art/ImportedFromUnreal/Nature/Omnis_Tree.obj",
  "Assets/Art/ImportedFromUnreal/Textures/Omnis_Nexus_Albedo.png"
];
for (const file of sourceAssets) if (!fs.existsSync(path.join(root, file))) errors.push("Missing imported art source: " + file);
if (errors.length) {
  console.error("OMNIS STATIC VALIDATION FAILED");
  for (const error of errors) console.error(" - " + error);
  process.exit(1);
}
console.log("OMNIS STATIC VALIDATION PASSED");
console.log("Verified project structure, scene bootstrap GUID, input, movement, objective, world-generation contract and source art.");
console.log("Note: this is a static check, not a Unity Editor build or runtime playtest.");
