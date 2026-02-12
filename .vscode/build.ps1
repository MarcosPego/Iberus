param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("Debug", "Release", "Dist", "Clean", "CleanBuild", "BuildAndRun")]
    [string]$Configuration = "Debug"
)

# Find Visual Studio installation
$vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $vswherePath)) {
    Write-Host "ERROR: Visual Studio Installer not found at $vswherePath" -ForegroundColor Red
    Write-Host "Please install Visual Studio with C++ workload." -ForegroundColor Red
    exit 1
}

# Get Visual Studio installation path
$vsPath = & $vswherePath -latest -property installationPath -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 2>$null

if (-not $vsPath) {
    Write-Host "ERROR: Visual Studio with C++ workload not found." -ForegroundColor Red
    Write-Host "Please install Visual Studio 2019 or 2022 with C++ development tools." -ForegroundColor Red
    exit 1
}

# Find MSBuild
$msbuild = Join-Path $vsPath "MSBuild\Current\Bin\MSBuild.exe"
if (-not (Test-Path $msbuild)) {
    $msbuild = Join-Path $vsPath "MSBuild\15.0\Bin\MSBuild.exe"
    if (-not (Test-Path $msbuild)) {
        Write-Host "ERROR: MSBuild not found in Visual Studio installation." -ForegroundColor Red
        exit 1
    }
}

# Get solution path
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$workspaceFolder = Split-Path -Parent $scriptPath
$solutionPath = Join-Path $workspaceFolder "Iberus.sln"

if (-not (Test-Path $solutionPath)) {
    Write-Host "ERROR: Solution file not found at $solutionPath" -ForegroundColor Red
    exit 1
}

# Build or clean
if ($Configuration -eq "Clean") {
    Write-Host "Cleaning solution..." -ForegroundColor Yellow
    & $msbuild $solutionPath /t:Clean /m /nologo /v:minimal
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Clean failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    } else {
        Write-Host "Clean succeeded!" -ForegroundColor Green
    }
} elseif ($Configuration -eq "CleanBuild") {
    Write-Host "Cleaning solution..." -ForegroundColor Yellow
    & $msbuild $solutionPath /t:Clean /m /nologo /v:minimal
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Clean failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    Write-Host "Building solution (Debug)..." -ForegroundColor Yellow
    & $msbuild $solutionPath /property:Configuration=Debug /property:Platform=x64 /t:Build /m /nologo /v:minimal
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    } else {
        Write-Host "Clean build succeeded!" -ForegroundColor Green
    }
} elseif ($Configuration -eq "BuildAndRun") {
    Write-Host "Building solution (Debug)..." -ForegroundColor Yellow
    & $msbuild $solutionPath /property:Configuration=Debug /property:Platform=x64 /t:Build /m /nologo /v:minimal
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    $sandboxExe = Join-Path $workspaceFolder "bin\Debug-windows-x86_64\Sandbox\Sandbox.exe"
    $sandboxDir = Split-Path $sandboxExe -Parent
    Write-Host "Running Sandbox..." -ForegroundColor Green
    Push-Location $sandboxDir
    & $sandboxExe
    Pop-Location
} else {
    Write-Host "Building solution ($Configuration)..." -ForegroundColor Yellow
    & $msbuild $solutionPath /property:Configuration=$Configuration /property:Platform=x64 /t:Build /m /nologo /v:minimal
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    } else {
        Write-Host "Build succeeded!" -ForegroundColor Green
    }
}
