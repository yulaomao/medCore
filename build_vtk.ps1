# VTK Build and Install Script
# This script builds VTK from source and installs it to thirtyPart/vtk
# Run this once, then medCore can use the pre-compiled VTK via find_package

param(
    [string]$Config = "Debug",
    [int]$Jobs = [Environment]::ProcessorCount
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$VtkSrcDir = Join-Path $ScriptDir "thirtyPart/vtk-src"
$VtkBuildDir = Join-Path $ScriptDir "build/vtk-build"
$VtkInstallDir = Join-Path $ScriptDir "thirtyPart/vtk"
$Qt6Path = "C:/Qt/6.9.1/msvc2022_64"

function Test-Prerequisites {
    Write-Host "[VTK] Checking prerequisites..." -ForegroundColor Cyan
    
    if (-not (Test-Path $VtkSrcDir)) {
        throw "VTK source not found at $VtkSrcDir"
    }
    
    if (-not (Test-Path (Join-Path $Qt6Path "lib"))) {
        throw "Qt6 not found at $Qt6Path. Please update the path in this script."
    }
    
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) {
        throw "CMake not found. Please install CMake or add it to PATH."
    }
    
    Write-Host "[VTK] Prerequisites OK" -ForegroundColor Green
}

function Configure-VTK {
    Write-Host "[VTK] Configuring VTK..." -ForegroundColor Cyan
    
    if (Test-Path $VtkBuildDir) {
        Remove-Item -Recurse -Force $VtkBuildDir
    }
    New-Item -ItemType Directory -Path $VtkBuildDir -Force | Out-Null
    
    Push-Location $VtkBuildDir
    try {
        $cmakeArgs = @(
            "-G", "Visual Studio 17 2022",
            "-A", "x64",
            "-Wno-dev",
            "-DCMAKE_INSTALL_PREFIX=$VtkInstallDir",
            "-DBUILD_SHARED_LIBS=ON",
            "-DVTK_QT_VERSION=6",
            "-DQt6_DIR=$Qt6Path/lib/cmake/Qt6",
            "-DVTK_BUILD_TESTING=OFF",
            "-DVTK_BUILD_EXAMPLES=OFF",
            "-DVTK_BUILD_DOCUMENTATION=OFF",
            "-DVTK_BUILD_ALL_MODULES=OFF",
            "-DVTK_ENABLE_REMOTE_MODULES=OFF",
            "-DVTK_ENABLE_WRAPPING=OFF",
            "-DVTK_GROUP_ENABLE_Qt=YES",
            "-DVTK_MODULE_ENABLE_VTK_CommonColor=YES",
            "-DVTK_MODULE_ENABLE_VTK_CommonCore=YES",
            "-DVTK_MODULE_ENABLE_VTK_FiltersSources=YES",
            "-DVTK_MODULE_ENABLE_VTK_GUISupportQt=YES",
            "-DVTK_MODULE_ENABLE_VTK_InteractionStyle=YES",
            "-DVTK_MODULE_ENABLE_VTK_RenderingCore=YES",
            "-DVTK_MODULE_ENABLE_VTK_RenderingOpenGL2=YES",
            $VtkSrcDir
        )
        
        cmake @cmakeArgs
        
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed with exit code $LASTEXITCODE"
        }
    } finally {
        Pop-Location
    }
    
    Write-Host "[VTK] Configuration complete" -ForegroundColor Green
}

function Build-VTK {
    Write-Host "[VTK] Building VTK (Config: $Config)..." -ForegroundColor Cyan
    
    Push-Location $VtkBuildDir
    try {
        cmake --build . --config $Config --parallel $Jobs
        
        if ($LASTEXITCODE -ne 0) {
            throw "VTK build failed with exit code $LASTEXITCODE"
        }
    } finally {
        Pop-Location
    }
    
    Write-Host "[VTK] Build complete" -ForegroundColor Green
}

function Install-VTK {
    Write-Host "[VTK] Installing VTK to $VtkInstallDir..." -ForegroundColor Cyan
    
    Push-Location $VtkBuildDir
    try {
        cmake --install . --config $Config
        
        if ($LASTEXITCODE -ne 0) {
            throw "VTK install failed with exit code $LASTEXITCODE"
        }
    } finally {
        Pop-Location
    }
    
    Write-Host "[VTK] Installation complete" -ForegroundColor Green
}

function Verify-Installation {
    Write-Host "[VTK] Verifying installation..." -ForegroundColor Cyan
    
    $vtkConfigFile = Join-Path $VtkInstallDir "lib/cmake/vtk/VTKConfig.cmake"
    if (Test-Path $vtkConfigFile) {
        Write-Host "[VTK] VTK successfully installed at $VtkInstallDir" -ForegroundColor Green
        return $true
    } else {
        Write-Host "[VTK] WARNING: VTKConfig.cmake not found at expected location" -ForegroundColor Yellow
        return $false
    }
}

# Main execution
try {
    Write-Host "`n=== VTK Build Script ===" -ForegroundColor Cyan
    Write-Host "Config: $Config" -ForegroundColor Gray
    Write-Host "Install Dir: $VtkInstallDir" -ForegroundColor Gray
    Write-Host ""
    
    Test-Prerequisites
    Configure-VTK
    Build-VTK
    Install-VTK
    Verify-Installation
    
    Write-Host "`n[SUCCESS] VTK build and installation complete!`n" -ForegroundColor Green
    Write-Host "Next steps:`n  1. Reconfigure medCore project (CMake configure)`n  2. Build medCore as normal`n" -ForegroundColor Cyan
    
} catch {
    Write-Host "`n[ERROR] $($_.Exception.Message)`n" -ForegroundColor Red
    exit 1
}
