param(
    [string]$UnrealRoot = $env:UE_ENGINE_ROOT,
    [string]$Configuration = "Development",
    [string]$Platform = "Win64"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$ProjectFile = Join-Path $ProjectRoot "Omnis.uproject"

if ([string]::IsNullOrWhiteSpace($UnrealRoot)) {
    throw "Provide -UnrealRoot or set UE_ENGINE_ROOT to a UE 5.6 installation."
}

$UAT = Join-Path $UnrealRoot "Engine/Build/BatchFiles/RunUAT.bat"
if (-not (Test-Path $UAT)) {
    throw "RunUAT.bat not found under UE_ENGINE_ROOT: $UnrealRoot"
}

& $UAT "BuildCookRun" "-project=$ProjectFile" "-noP4" "-build" "-platform=$Platform" "-clientconfig=$Configuration" "-serverconfig=$Configuration"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
