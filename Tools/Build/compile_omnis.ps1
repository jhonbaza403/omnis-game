param(
    [string]$UnrealRoot = $env:UE_ENGINE_ROOT,
    [string]$Configuration = "Development",
    [string]$Platform = "Win64"
 )

$ErrorActionPreference = "Stop"

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$ProjectFile = Join-Path $ProjectRoot "Omnis.uproject"

if ([string]::IsNullOrWhiteSpace($UnrealRoot)) {
    throw "Provide -UnrealRoot or set UE_ENGINE_ROOT to a UE 5.8 installation."
}

$BuildVersion = Join-Path $UnrealRoot "Engine/Build/Build.version"
$RunUBT = Join-Path $UnrealRoot "Engine/Build/BatchFiles/RunUBT.bat"

if (-not (Test-Path $BuildVersion)) {
    throw "Unreal Engine Build.version not found: $BuildVersion"
}
if (-not (Test-Path $RunUBT)) {
    throw "RunUBT.bat not found under UE_ENGINE_ROOT: $UnrealRoot"
}

$Version = Get-Content $BuildVersion -Raw | ConvertFrom-Json
if ("$($Version.MajorVersion).$($Version.MinorVersion)" -ne "5.8") {
    throw "Wrong Unreal Engine version: detected $($Version.MajorVersion).$($Version.MinorVersion).$($Version.PatchVersion); required 5.8.x"
}

& $RunUBT "-Mode=UnrealHeaderTool" "-Target=OmnisEditor $Platform $Configuration" "-Project=$ProjectFile"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $RunUBT "OmnisEditor" $Platform $Configuration "-Project=$ProjectFile" "-WaitMutex"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $RunUBT "Omnis" $Platform $Configuration "-Project=$ProjectFile" "-WaitMutex"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "UE 5.8 UHT + UBT + module link + Editor/Game build completed successfully."
