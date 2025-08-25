<#
    This is a PowerShell version of the original make-psiplus-cmake-64-vs.cmd batch script.
    It attempts to preserve the logic and structure, but uses idiomatic PowerShell where possible.
    Some Windows environment variable conventions are replaced by PowerShell variables.
    User input is handled with Read-Host.
    Clean up, error handling, and commands are PowerShell-native.
#>

Write-Host "PSI-PLUS BUILD SCRIPT`n"

# ====== GLOBAL VARIABLES ======
# CPU count
$CPUCOUNT = (Get-WmiObject -Class Win32_ComputerSystem).NumberOfLogicalProcessors
# Build with debug information OFF/ON
$USEDEBUG = "OFF"
# Production build OFF/ON
$USEPRODUCTION = "OFF"
# Pauses during build OFF/ON
$USEPAUSES = "OFF"
# Build psimedia ON/OFF
$BUILD_PSIMEDIA = "ON"

Write-Host "CPUs: $CPUCOUNT"
Write-Host "DEBUG: $USEDEBUG"
Write-Host "PAUSES: $USEPAUSES"
Write-Host ""

# ====== ABSOLUTE PATHS ======
$BUILDDIR = "C:\build"
$QTPREFIX = "C:\Qt"
$QTVER = 6
$VCVARS = "C:\MSVC2022\VC\Auxiliary\Build\vcvarsall.bat"
$ISS_COMPIL = "C:\Program Files (x86)\Inno Setup 6\Compil32.exe"
$GITBIN_DIR = Join-Path $BUILDDIR "Git\bin"
$GIT_EXECUTABLE = Join-Path $GITBIN_DIR "git.exe"
$PKG_CONFIG_EXECUTABLE = "C:/gstreamer/1.0/msvc_x86_64/bin/pkg-config.exe"
$CCACHE_BIN_DIR = Join-Path $BUILDDIR "ccache"

# ====== RELATIVE PATHS ======
$PARENT_DIR = Get-Location
$LOGDIR = Join-Path $PARENT_DIR "logs"
$BUILDLOG = Join-Path $LOGDIR "build.log"
$ERRLOGPREF = Join-Path $LOGDIR "psi-plus-vs-errors"
$PSIDIST = $BUILDDIR
$PSIPATH = Join-Path $PSIDIST "psi"
$BASEDIST = Join-Path $PSIDIST "psi-build-src"
$LANGSPATH = Join-Path $PSIDIST "translations"
$RESPATH = Join-Path $PSIDIST "resources"
$PLUGSPATH = Join-Path $PSIDIST "plugins"
$MEDIAPATH = Join-Path $PSIDIST "psimedia"
$WORKDIR = $PSIDIST
$GITBIN = $GIT_EXECUTABLE
$CMAKEDIR = Join-Path $QTPREFIX "Tools\Cmake_64"
$CMAKEBIN = Join-Path $CMAKEDIR "bin\cmake.exe"
$JOMPATH = Join-Path $QTPREFIX "Tools\QtCreator\bin\jom"
$JOM = Join-Path $JOMPATH "jom.exe"

# ==== DO NOT CHANGE THIS PLEASE ====
$binTemplate = "win64"
$webkitTemplate = "webengine"
$qtTemplate = "qt$QTVER"

# ====== win64 VARIABLES ======
$ARCHNAME = $binTemplate
# ====== Qt win64 VARIABLES ======
$QTDIR = Join-Path $QTPREFIX "6.9.1\msvc2022_64"
$DEFAULT_CMAKE_FLAGS = @("-DUSE_CCACHE=ON",
                       "-DUSE_QT6=ON",
                       "-DGIT_EXECUTABLE=`"$GIT_EXECUTABLE`"",
                       "-DCMAKE_INSTALL_PREFIX=`"$PSIDIST\Installer`"",
                       "-DPSI_PLUS=ON")
$CMAKE_BTYPE = "-G`"NMake Makefiles JOM`""
$EXITMARK = 0

# Start logging
New-Item -ItemType Directory -Force -Path $LOGDIR | Out-Null
"Logging started" | Out-File -Encoding UTF8 -FilePath $BUILDLOG
"_" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG

# ================== MAIN LOGIC ==================
function FetchSources {
    Set-Location $BUILDDIR
    # Download and prepare sources using cmake script
    & "$CMAKEDIR\bin\cmake.exe" -DGIT_EXECUTABLE="$GIT_EXECUTABLE" -DBUILD_DIR="C:/build" -P "$BUILDDIR\Prepare-psi.cmake"
    PrepareSrc
}

function PrepareSrc {
    if (-not (Test-Path $BASEDIST)) {
        Write-Host "$BASEDIST not found. Please run script again and download psi+ sources"
        ExitLabel
    }
    Set-Location $WORKDIR
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    Selection
}

function Selection {
    Write-Host "Choose start point:"
    Write-Host "-1--Build All"
    Write-Host "-2--Build only Qt$QTVER"
    Write-Host "-3--Start only Qt$QTVER $webkitTemplate"
    Write-Host "-4--Build only plugins"
    Write-Host "-5--Exit"
    $BUILD_VARIANT = Read-Host "Select 1-5 and Press Enter"
    switch ($BUILD_VARIANT) {
        "1" { BuildAll }
        "2" { BuildBasic }
        "3" { BuildWeb }
        "4" { BuildPlugs }
        default { Finish }
    }
}

function BuildAll {
    Write-Host "Normal Qt$QTVER version"
    $script:ISWEBKIT = "basic"
    SetCommonVars
    MakeIt
    $script:ISWEBKIT = "webengine"
    Write-Host "Webengine Qt$QTVER version"
    MakeIt
    Write-Host "Plugins"
    MakePlugins
    Write-Host "Installer"
    CreateSetup
}

function BuildBasic {
    Write-Host "Normal Qt$QTVER version"
    $script:ISWEBKIT = "basic"
    SetCommonVars
    MakeIt
}

function BuildWeb {
    Write-Host "Webengine Qt$QTVER version"
    $script:ISWEBKIT = "webengine"
    SetCommonVars
    MakeIt
}

function BuildPlugs {
    Write-Host "Plugins"
    SetCommonVars
    MakePlugins
}

function Finish {
    $script:EXITMARK = 1
}

function Invoke-CmdScript {
  param(
    [String] $scriptName
  )
  $cmdLine = """$scriptName"" $args & set"
  & $Env:SystemRoot\system32\cmd.exe /c $cmdLine |
  select-string '^([^=]*)=(.*)$' | foreach-object {
    $varName = $_.Matches[0].Groups[1].Value
    $varValue = $_.Matches[0].Groups[2].Value
    set-item Env:$varName $varValue
  }
}

function SetCommonVars {
    $script:ARCH = $ARCHNAME
    $script:BTYPE = "W64"
    "* Building $($script:BTYPE)-=$ISWEBKIT-$qtTemplate psi+ binary *" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    "_" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    $WINVARS = @(
        "$env:SystemRoot\system32"
        "$env:SystemRoot"
        "$env:SystemRoot\System32\Wbem"
        "$env:SystemRoot\System32\WindowsPowerShell\v1.0\"
    ) -join ";"
    $env:PATH = $WINVARS
    Invoke-CmdScript $VCVARS amd64
    $env:PATH += ";$QTDIR;$QTDIR\bin;$JOMPATH;"
    $env:PATH += "$env:PSI_SDK_MSVC_WIN64;$env:PSI_SDK_MSVC_WIN64\bin;"
    $env:PATH += "$env:PSI_SDK_MSVC_WIN64\lib;$env:PSI_SDK_MSVC_WIN64\include;$CCACHE_BIN_DIR;"
    $env:PATH | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
}

function MakeIt {
    Set-Location $WORKDIR
    $BDIR = Join-Path $PSIDIST "psi-plus_build"
    if (Test-Path $BDIR) { Remove-Item -Recurse -Force $BDIR }
    New-Item -ItemType Directory -Path $BDIR | Out-Null
    Set-Location $BDIR

    $CMAKE_FLAGS = @("-DCHAT_TYPE=$ISWEBKIT",
                   "-DBUNDLED_IRIS_ALL=ON",
                   "-DVERBOSE_PROGRAM_NAME=ON")
    if ($ISWEBKIT -eq "basic") { $CMAKE_FLAGS += "-DTRANSLATIONS_DIR=`"$LANGSPATH\translations`"" }
    if ($USEDEBUG -eq "ON") { $CMAKE_FLAGS += "-DCMAKE_BUILD_TYPE=Debug" }
    $CMAKE_FLAGS += "-DPRODUCTION=$USEPRODUCTION"
    $ARGSLIST = @("$CMAKE_BTYPE",
                "-S`"$PSIPATH`"",
                "-B`"$BDIR`"")
    foreach ($element in $DEFAULT_CMAKE_FLAGS) {
        $ARGSLIST += "$element"
    }
    foreach ($element in $CMAKE_FLAGS) {
        $ARGSLIST += "$element"
    }
    Write-Host "$CMAKEBIN $ARGSLIST"
    & $CMAKEBIN $ARGSLIST
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    "cmake with flags $CMAKE_FLAGS finished" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    "$($script:BTYPE) =$ISWEBKIT configured" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    Set-Location $BDIR
    & $CMAKEBIN --build `"$BDIR`" --target all --parallel $CPUCOUNT
    "$($script:BTYPE) =$ISWEBKIT compiled" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }

    if ($USEPRODUCTION -eq "ON") {
        & $CMAKEBIN --build `"$BDIR`" --target prepare-bin --target prepare-bin-libs --target windeploy
        if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    }
    & $CMAKEBIN --build `"$BDIR`" --target install
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    Set-Location $WORKDIR
    if (Test-Path $BDIR) { Remove-Item -Recurse -Force $BDIR }
}

function MakePlugins {
    Set-Location $WORKDIR
    $BDIR = Join-Path $PSIDIST "psi-plugins_build"
    if (Test-Path $BDIR) { Remove-Item -Recurse -Force $BDIR }
    New-Item -ItemType Directory -Path $BDIR | Out-Null
    Set-Location $BDIR

    $CMAKE_FLAGS = @("$DEFAULT_CMAKE_FLAGS",
                   "-DONLY_PLUGINS=ON",
                   "-DBUILD_DEV_PLUGINS=$BPLUGS",
                   "-DBUNDLED_OMEMO_C_ALL=ON",
                   "-DBUILD_PSIMEDIA=$BUILD_PSIMEDIA",
                   "-DPKG_CONFIG_EXECUTABLE=`"$PKG_CONFIG_EXECUTABLE`"")
    if ($USEDEBUG -eq "ON") { $CMAKE_FLAGS += " -DCMAKE_BUILD_TYPE=Debug" }
    $ARGSLIST = @("$CMAKE_BTYPE",
                "-S`"$BASEDIST`"",
                "-B`"$BDIR`"")
    foreach ($element in $DEFAULT_CMAKE_FLAGS) {
        $ARGSLIST += "$element"
    }
    foreach ($element in $CMAKE_FLAGS) {
        $ARGSLIST += "$element"
    }
    & $CMAKEBIN $ARGSLIST
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    "cmake with flags $CMAKE_FLAGS finished" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    & $CMAKEBIN --build `"$BDIR`" --target all --parallel $CPUCOUNT
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    & $CMAKEBIN --build `"$BDIR`" --target install
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    Set-Location $WORKDIR
    if (Test-Path $BDIR) { Remove-Item -Recurse -Force $BDIR }
}

function CreateSetup {
    $ans6 = Read-Host "Do you want to create psi-plus installer [y(default)/n]"
    if ([string]::IsNullOrWhiteSpace($ans6)) { $ans6 = "y" }
    if ($ans6 -ne "n") {
        $INSTALLERDIR = Join-Path $BUILDDIR "Installer"
        & "$ISS_COMPIL" /cc "$INSTALLERDIR\psi-plus-setup-64.iss"
    }
}
# User input: fetch sources
$fetch = Read-Host "Do you want to download/update psi+ sources [y(default)/n]"
if ([string]::IsNullOrWhiteSpace($fetch)) { $fetch = "y" }
if ($fetch -eq "n") {
    PrepareSrc
} else {
    FetchSources
}

while ($EXITMARK -ne 1) {
    Selection
}
