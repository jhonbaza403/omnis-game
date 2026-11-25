[CmdletBinding()]
param(
    [string]$UnrealRoot = $env:UE_ENGINE_ROOT,
    [ValidateSet("Development", "DebugGame", "Shipping")]
    [string]$Configuration = "Development",
    [ValidateSet("Win64")]
    [string]$Platform = "Win64",
    [switch]$IncludeServer
)

$ErrorActionPreference = "Stop"
$canonicalBuilder = Join-Path $PSScriptRoot "build_omnis_windows.ps1"
if (!(Test-Path $canonicalBuilder -PathType Leaf)) {
    throw "Canonical Windows build script not found: $canonicalBuilder"
}

# Backward-compatible entry point. All Windows build logic lives in one script.
$arguments = @(
    "-EngineRoot", $UnrealRoot,
    "-Configuration", $Configuration,
    "-Platform", $Platform
)
if ($IncludeServer) {
    $arguments += "-IncludeServer"
}

& $canonicalBuilder @arguments
