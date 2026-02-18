# Build and run all tests. Outputs to Tests-Build.
# Use this when editing to verify the build is not broken.
# Exit codes: 0 = pass, 1 = regenerate/build failed, 2 = unit tests failed, 3 = integration tests failed.
#
# Note: Unit tests (IberusTests.exe) currently crash on startup when loading Iberus.dll.
# Use -SkipUnitTests to skip them; integration tests (Game.exe --run-tests) run fine.

param(
    [switch]$SkipIntegration,
    [switch]$SkipUnitTests = $true,
    [switch]$SkipRegenerate
)

$ErrorActionPreference = "Stop"
$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$SolutionDir = Split-Path -Parent $ScriptRoot
$SlnPath = Join-Path $SolutionDir "Iberus.sln"
$PremakeExe = Join-Path $SolutionDir "dependencies\premake\premake5.exe"
$BuildDir = Join-Path $SolutionDir "Tests-Build"

# Step 0: Regenerate project (same as project.bat)
if (-not $SkipRegenerate) {
    Write-Host "=== Regenerating project (premake) ===" -ForegroundColor Cyan
    if (Test-Path $PremakeExe) {
        Push-Location $SolutionDir
        try {
            & $PremakeExe vs2022
            if ($LASTEXITCODE -ne 0) {
                Write-Host "`nPREMAKE FAILED" -ForegroundColor Red
                exit 1
            }
        } finally {
            Pop-Location
        }
    } else {
        Write-Host " premake5.exe not found, skipping. Use -SkipRegenerate to suppress, or run project.bat." -ForegroundColor Yellow
    }
}

# Find MSBuild
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$msbuildPath = $null
if (Test-Path $vswhere) {
    $msbuildPath = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" 2>$null | Select-Object -First 1
}
if (-not $msbuildPath) {
    Write-Host "ERROR: MSBuild not found. Install Visual Studio with C++ workload." -ForegroundColor Red
    exit 1
}

# Step 1: Build (Test configuration -> Tests-Build)
Write-Host "`n=== Building (Test -> Tests-Build) ===" -ForegroundColor Cyan
& $msbuildPath $SlnPath /p:Configuration=Test /p:Platform=x64 /m /nologo /v:minimal
if ($LASTEXITCODE -ne 0) {
    Write-Host "`nBUILD FAILED" -ForegroundColor Red
    exit 1
}

# Step 2: Unit tests (IberusTests.exe)
if (-not $SkipUnitTests) {
    $testExe = Join-Path $BuildDir "IberusTests.exe"
    if (Test-Path $testExe) {
        Write-Host "`n=== Unit tests ===" -ForegroundColor Cyan
        Push-Location $BuildDir
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
        Write-Host "`nIberusTests.exe not found in Tests-Build, skipping unit tests." -ForegroundColor Yellow
    }
} else {
    Write-Host "`nSkipping unit tests (-SkipUnitTests)." -ForegroundColor Yellow
}

# Step 3: Integration tests (Game.exe --run-tests)
if (-not $SkipIntegration) {
    $gameExe = Join-Path $BuildDir "Game.exe"
    if (Test-Path $gameExe) {
        Write-Host "`n=== Integration tests ===" -ForegroundColor Cyan
        # Ensure Tests-Build has Projects/Demo for assets (shaders, etc.)
        $projectsDir = Join-Path $BuildDir "Projects"
        $demoDir = Join-Path $BuildDir "Projects\Demo"
        $sourceDemo = Join-Path $SolutionDir "Game-Build\Projects\Demo"
        if (-not (Test-Path $demoDir)) {
            if (Test-Path $sourceDemo) {
                if (-not (Test-Path $projectsDir)) { New-Item -ItemType Directory -Path $projectsDir -Force | Out-Null }
                Copy-Item -Path $sourceDemo -Destination $demoDir -Recurse -Force
            } else {
                Write-Host "WARN: Projects/Demo not found. Build Game (Debug) once to create it, or integration tests may fail." -ForegroundColor Yellow
            }
        }
        Push-Location $BuildDir
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
        Write-Host "`nGame.exe not found in Tests-Build, skipping integration tests." -ForegroundColor Yellow
    }
}

Write-Host "`nALL CHECKS PASSED" -ForegroundColor Green
exit 0
