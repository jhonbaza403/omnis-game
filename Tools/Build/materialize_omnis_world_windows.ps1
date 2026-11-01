# Safely materialize the Omnis native Open World content with Unreal Engine 5.8.
# Existing maps are never overwritten; existing valid mesh assets are preserved by the Python importer.
[CmdletBinding()]
param(
    [string]$EngineRoot = $env:UE_ENGINE_ROOT
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$projectFile = Join-Path $repoRoot "Omnis.uproject"
$pythonScript = Join-Path $repoRoot "Tools/Editor/MaterializeOmnisPremiumWorld.py"
$editorCmd = Join-Path $EngineRoot "Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
$mapFile = Join-Path $repoRoot "Content/Omnis/Maps/Omnis_OpenWorld.umap"
$artRoot = Join-Path $repoRoot "Content/Omnis/Art"
$materialsRoot = Join-Path $repoRoot "Content/Omnis/Materials"

if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw "Set UE_ENGINE_ROOT to your Unreal Engine 5.8 installation or pass -EngineRoot."
}
$EngineRoot = (Resolve-Path $EngineRoot).Path
$versionFile = Join-Path $EngineRoot "Engine/Build/Build.version"
if (!(Test-Path $versionFile -PathType Leaf)) { throw "Build.version not found: $versionFile" }
$version = Get-Content $versionFile -Raw | ConvertFrom-Json
if ([int]$version.MajorVersion -ne 5 -or [int]$version.MinorVersion -ne 8) {
    throw "Expected Unreal Engine 5.8.x; found $($version.MajorVersion).$($version.MinorVersion).$($version.PatchVersion)."
}
if (!(Test-Path $projectFile -PathType Leaf)) { throw "Omnis.uproject not found: $projectFile" }
if (!(Test-Path $pythonScript -PathType Leaf)) { throw "World materializer not found: $pythonScript" }
if (!(Test-Path $editorCmd -PathType Leaf)) { throw "UnrealEditor-Cmd.exe not found: $editorCmd" }

if (Test-Path $mapFile) {
    throw "Refusing to overwrite existing map: $mapFile. Back up and review it manually before any migration."
}

$runId = Get-Date -Format "yyyyMMddTHHmmssfffZ"
$logDirectory = Join-Path $repoRoot "Saved/Logs"
New-Item -ItemType Directory -Force -Path $logDirectory | Out-Null
$logFile = Join-Path $logDirectory "omnis_world_materialization_$runId.log"

& $editorCmd $projectFile -unattended -nop4 -nosplash -nullrhi "-ExecutePythonScript=$pythonScript" -log 2>&1 |
    Tee-Object -FilePath $logFile
if ($LASTEXITCODE -ne 0) {
    throw "Unreal world materialization failed with exit code $LASTEXITCODE. Log: $logFile"
}

if (!(Test-Path $mapFile -PathType Leaf)) { throw "Native map was not created: $mapFile" }
$nativeAssets = @(Get-ChildItem $artRoot -Recurse -File -Filter "*.uasset" -ErrorAction SilentlyContinue).Count
$nativeMaterials = @(Get-ChildItem $materialsRoot -Recurse -File -Filter "*.uasset" -ErrorAction SilentlyContinue).Count
$logText = Get-Content $logFile -Raw
if ($nativeAssets -lt 63) { throw "Expected at least 63 native art assets; found $nativeAssets." }
if ($nativeMaterials -lt 10) { throw "Expected at least 10 native material assets; found $nativeMaterials." }
if ($logText -notmatch "REAL PREMIUM WORLD MATERIALIZED") {
    throw "The Unreal script did not emit its materialization success marker. Log: $logFile"
}

"Native map: $mapFile"
"Native art assets: $nativeAssets"
"Native material assets: $nativeMaterials"
"Log: $logFile"
