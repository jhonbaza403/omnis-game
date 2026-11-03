[CmdletBinding()]
param(
    [string]$EngineRoot = $env:UE_ENGINE_ROOT,
    [ValidateSet("Development", "DebugGame", "Shipping")]
    [string]$Configuration = "Development",
    [ValidateSet("Win64")]
    [string]$Platform = "Win64",
    [switch]$IncludeServer
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$projectFile = Join-Path $repoRoot "Omnis.uproject"

if ([string]::IsNullOrWhiteSpace($EngineRoot)) {
    throw "Set UE_ENGINE_ROOT to your Unreal Engine 5.8 installation or pass -EngineRoot."
}
$EngineRoot = (Resolve-Path $EngineRoot).Path
$versionFile = Join-Path $EngineRoot "Engine/Build/Build.version"
$buildTool = Join-Path $EngineRoot "Engine/Build/BatchFiles/Build.bat"

if (!(Test-Path $projectFile -PathType Leaf)) { throw "Omnis.uproject not found: $projectFile" }
if (!(Test-Path $versionFile -PathType Leaf)) { throw "Build.version not found: $versionFile" }
if (!(Test-Path $buildTool -PathType Leaf)) { throw "Build.bat not found: $buildTool" }

$version = Get-Content $versionFile -Raw | ConvertFrom-Json
if ([int]$version.MajorVersion -ne 5 -or [int]$version.MinorVersion -ne 8) {
    throw "Expected Unreal Engine 5.8.x; found $($version.MajorVersion).$($version.MinorVersion).$($version.PatchVersion)."
}

$runId = [DateTime]::UtcNow.ToString("yyyyMMddTHHmmssfffZ")
$logDirectory = Join-Path $repoRoot "Saved/Logs"
New-Item -ItemType Directory -Force -Path $logDirectory | Out-Null
$logFile = Join-Path $logDirectory "omnis_ue58_windows_$runId.log"

$targets = @(
    @{ Name = "OmnisEditor"; Platform = $Platform },
    @{ Name = "Omnis"; Platform = $Platform }
)
if ($IncludeServer) {
    $targets += @{ Name = "OmnisServer"; Platform = $Platform }
}

"Omnis UE 5.8 build | Version $($version.MajorVersion).$($version.MinorVersion).$($version.PatchVersion) | $runId" |
    Tee-Object -FilePath $logFile

foreach ($target in $targets) {
    $arguments = @(
        $target.Name,
        $target.Platform,
        $Configuration,
        "-Project=$projectFile",
        "-WaitMutex",
        "-NoHotReloadFromIDE"
    )
    ">>> Building $($target.Name) $($target.Platform) $Configuration" |
        Tee-Object -FilePath $logFile -Append

    & $buildTool @arguments 2>&1 | Tee-Object -FilePath $logFile -Append
    if ($LASTEXITCODE -ne 0) {
        throw "UBT failed for $($target.Name) with exit code $LASTEXITCODE. Log: $logFile"
    }
}

"UE 5.8 UBT builds completed for: $($targets.Name -join ', ')" |
    Tee-Object -FilePath $logFile -Append
"Build log: $logFile"
