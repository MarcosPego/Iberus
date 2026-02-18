# Build and run all checks for Iberus.
# Exit codes: 0 = pass, 1 = build failed, 2 = unit tests failed, 3 = integration tests failed.

param(
    [switch]$SkipIntegration
)

$ErrorActionPreference = "Stop"
$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$SolutionDir = Split-Path -Parent $ScriptRoot
$SlnPath = Join-Path $SolutionDir "Iberus.sln"

# Find MSBuild
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$msbuildPath = $null
if (Test-Path $vswhere) {
    $msbuildPath = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" 2>$null | Select-Object -First 1
}
if (-not $msbuildPath) {
    Write-Host "ERROR: MSBuild not found. Install Visual Studio with C++ workload."
    exit 1
}

# Step 1: Build
Write-Host "=== Building ===" -ForegroundColor Cyan
& $msbuildPath $SlnPath /p:Configuration=Debug /p:Platform=x64 /m /nologo /v:minimal
if ($LASTEXITCODE -ne 0) {
    Write-Host "`nBUILD FAILED" -ForegroundColor Red
    exit 1
}

# Step 2: Unit tests (IberusTests.exe)
$testExe = Join-Path $SolutionDir "Game-Build\IberusTests.exe"
if (Test-Path $testExe) {
    Write-Host "`n=== Unit tests ===" -ForegroundColor Cyan
    Push-Location (Join-Path $SolutionDir "Game-Build")
    try {
        & $testExe
        if ($LASTEXITCODE -ne 0) {
            Write-Host "`nUNIT TESTS FAILED" -ForegroundColor Red
            exit 2
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Host "`nIberusTests.exe not found, skipping unit tests." -ForegroundColor Yellow
}

# Step 3: Integration tests (Game.exe --run-tests)
if (-not $SkipIntegration) {
    $gameExe = Join-Path $SolutionDir "Game-Build\Game.exe"
    if (Test-Path $gameExe) {
        Write-Host "`n=== Integration tests ===" -ForegroundColor Cyan
        Push-Location (Join-Path $SolutionDir "Game-Build")
        try {
            & $gameExe --run-tests
            if ($LASTEXITCODE -ne 0) {
                Write-Host "`nINTEGRATION TESTS FAILED" -ForegroundColor Red
                exit 3
            }
        } finally {
            Pop-Location
        }
    } else {
        Write-Host "`nGame.exe not found, skipping integration tests." -ForegroundColor Yellow
    }
}

Write-Host "`nALL CHECKS PASSED" -ForegroundColor Green
exit 0
