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
# Use Ninja or JOM
$USENINJA = "ON"


Write-Host "CPUs: $CPUCOUNT"
Write-Host "DEBUG: $USEDEBUG"
Write-Host "PAUSES: $USEPAUSES"

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
$NINJAPATH = Join-Path $QTPREFIX "Tools\Ninja"
#$JOM = Join-Path $JOMPATH "jom.exe"

# ====== GIT URLS ======
$PSI_GIT_PREFIX = "https://github.com/psi-im"
$PSI_GIT_URL = "$PSI_GIT_PREFIX/psi.git"
$PLUGINS_GIT_URL = "$PSI_GIT_PREFIX/plugins.git"
$TRANSLATIONS_GIT_URL = "https://github.com/psi-plus/psi-plus-l10n.git"
$PSIMEDIA_GIT_URL = "$PSI_GIT_PREFIX/psimedia.git"
$RESOURCES_GIT_URL = "$PSI_GIT_PREFIX/resources.git"

# ==== DO NOT CHANGE THIS PLEASE ====
$binTemplate = "win64"
$webkitTemplate = "webengine"
$qtTemplate = "qt$QTVER"

# ====== win64 VARIABLES ======
$ARCHNAME = $binTemplate
# ====== Qt win64 VARIABLES ======
$QTVERSION = "6.11.2"
$QTARCH = "msvc2022_64"
$QTDIR = Join-Path $QTPREFIX "$QTVERSION\$QTARCH"
$DEFAULT_CMAKE_FLAGS = @("-DUSE_CCACHE=ON",
                       "-DUSE_QT6=ON",
                       "-DGIT_EXECUTABLE=$GIT_EXECUTABLE",
                       "-DCMAKE_INSTALL_PREFIX=$PSIDIST\Installer",
                       "-DPSI_PLUS=ON")
if ($USENINJA -eq "ON")
{
    $CMAKE_BTYPE = @("-G", "Ninja")
}
else
{
    $CMAKE_BTYPE = @("-G", "NMake Makefiles JOM")
}
$EXITMARK = 0

$mainRepo = @{
    Url       = $PSI_GIT_URL
    LocalPath = $PSIPATH
    Branch    = "master"
}

$dependencies = @(
    @{
        Url             = $PLUGINS_GIT_URL
        LocalPath       = $PLUGSPATH
        Branch          = "master"
        TargetSubFolder = "plugins"
        SourceSubFolder = ""
    },
    @{
        Url             = $TRANSLATIONS_GIT_URL
        LocalPath       = $LANGSPATH
        Branch          = "master"
        TargetSubFolder = "translations"
        SourceSubFolder = "translations"
    },
    @{
        Url             = $PSIMEDIA_GIT_URL
        LocalPath       = $MEDIAPATH
        Branch          = "master"
        TargetSubFolder = "plugins/generic/psimedia"
    },
    @{
        Url             = $RESOURCES_GIT_URL
        LocalPath       = $RESPATH
        Branch          = "master"
        TargetSubFolder = ""
        SourceSubFolder = ""
    }
)

# Start logging
New-Item -ItemType Directory -Force -Path $LOGDIR | Out-Null
"Logging started" | Out-File -Encoding UTF8 -FilePath $BUILDLOG
"_" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG

# ================== MAIN LOGIC ==================
function Sync-GitRepository {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$Url,
        [Parameter(Mandatory)]
        [string]$LocalPath,
        [Parameter(Mandatory)]
        [string]$Branch = "master"
    )
    Write-Host "---> Processing repository: $Url" -ForegroundColor Cyan
    $gitDirectory = Join-Path $LocalPath ".git"
    # Клонирование нового репозитория
    if (-not (Test-Path $gitDirectory)) {
        Write-Host "Repository not found. Clonning..." -ForegroundColor Yellow
        if (Test-Path $LocalPath) {
            Remove-Item $LocalPath -Recurse -Force
        }
        $parentDirectory = Split-Path $LocalPath -Parent
        New-Item -ItemType Directory -Path $parentDirectory -Force | Out-Null
        & $GIT_EXECUTABLE clone --recursive $Url $LocalPath
        if ($LASTEXITCODE -ne 0) {
            throw "Error clonning repository: $Url"
        }
        return
    }
    # Update existing repository
    Write-Host "Repository found. Cleanup and update..." -ForegroundColor Yellow
    Push-Location $LocalPath
    try {
        Invoke-GitCommand @("reset", "--hard", "HEAD")
        Invoke-GitCommand @("clean", "-fdx")
        Invoke-GitCommand @("checkout", $Branch)
        Invoke-GitCommand @("pull", "origin", $Branch)
        Invoke-GitCommand @("submodule", "update", "--init", "--recursive", "--force")
        # Update submodules
        & $GIT_EXECUTABLE submodule foreach --recursive `
            "git reset --hard HEAD && git clean -fdx"
        if ($LASTEXITCODE -ne 0) {
            throw "Error cleaning submodules in repository: $LocalPath"
        }
    }
    finally {
        Pop-Location
    }
}


function Invoke-GitCommand {
    param(
        [Parameter(Mandatory)]
        [string[]]$Arguments
    )
    Write-Host "git $($Arguments -join ' ')" -ForegroundColor DarkGray
    & $GIT_EXECUTABLE @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Git-command failed ${LASTEXITCODE}: git $($Arguments -join ' ')"
    }
}

function Get-ProjectVersion {
    param(
        [Parameter(Mandatory)]
        [string]$RepositoryPath
    )
    Write-Host "---> Detect Main project version..." -ForegroundColor Cyan
    Push-Location $RepositoryPath
    try {
        $latestTag = (& $GIT_EXECUTABLE describe --tags --abbrev=0 2>$null).Trim()
        if ([string]::IsNullOrWhiteSpace($latestTag)) {
            Write-Host "Tags not found. Using 1.5 version as default" -ForegroundColor Yellow
            $baseVersion = "1.5"
            $revision = (& $GIT_EXECUTABLE rev-list --count HEAD).Trim()
        }
        else {
            $baseVersion = $latestTag -replace "^v", ""
            $revision = (& $GIT_EXECUTABLE rev-list "$latestTag..HEAD" --count).Trim()
        }
        if ($LASTEXITCODE -ne 0) {
            throw "Cannot get Git revision number"
        }
        $fullVersion = "$baseVersion.$revision"
        Write-Host "Formed version: $fullVersion" -ForegroundColor Green
        return $fullVersion
    }
    finally {
        Pop-Location
    }
}


function Copy-ProjectSources {
    param(
        [Parameter(Mandatory = $true)]
        [string]$TargetDirectory,

        [Parameter(Mandatory = $true)]
        [hashtable[]]$Projects
    )
    Write-Host "---> Copying sources to $TargetDirectory..." `
        -ForegroundColor Cyan
    if (Test-Path $TargetDirectory) {
        Remove-Item $TargetDirectory -Recurse -Force
    }
    New-Item `
        -ItemType Directory `
        -Path $TargetDirectory `
        -Force | Out-Null
    foreach ($project in $Projects) {
        $source = $project.LocalPath
        if (-not [string]::IsNullOrWhiteSpace(
            $project.SourceSubFolder
        )) {
            $source = Join-Path `
                $source `
                $project.SourceSubFolder
        }
        if (-not (Test-Path $source)) {
            throw "Input path not found: $source"
        }
        if ([string]::IsNullOrWhiteSpace(
            $project.TargetSubFolder
        )) {
            $destination = $TargetDirectory
        }
        else {
            $destination = Join-Path `
                $TargetDirectory `
                $project.TargetSubFolder
        }
        New-Item `
            -ItemType Directory `
            -Path $destination `
            -Force | Out-Null
        Write-Host "Copying:" -ForegroundColor DarkGray
        Write-Host "  from: $source" -ForegroundColor DarkGray
        Write-Host "  to:   $destination" -ForegroundColor DarkGray
        Get-ChildItem -Path $source -Force |
            Where-Object {
                $_.Name -ne ".git"
            } |
            Copy-Item `
                -Destination $destination `
                -Recurse `
                -Force
    }
}

function Save-BuildVersion {
    param(
        [Parameter(Mandatory)]
        [string]$TargetDirectory,
        [Parameter(Mandatory)]
        [string]$Version
    )
    $versionFilePath = Join-Path $TargetDirectory "version.txt"
    $Version | Out-File `
        -FilePath $versionFilePath `
        -Encoding utf8
    $global:BuildVersion = $Version
    $env:BUILD_VERSION = $Version
    Write-Host "Version file seved to: $versionFilePath" `
        -ForegroundColor Green
}


function Sync-ProjectsAndBuildVersion {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$TargetDirectory,
        [Parameter(Mandatory)]
        [hashtable]$MainProject,
        [Parameter()]
        [hashtable[]]$DependencyProjects = @()
    )
    # Sync main repository
    $mainBranch = if (
        [string]::IsNullOrWhiteSpace($MainProject.Branch)
    ) {
        "master"
    }
    else {
        $MainProject.Branch
    }
    Sync-GitRepository `
        -Url $MainProject.Url `
        -LocalPath $MainProject.LocalPath `
        -Branch $mainBranch
    # Sync dependencies
    foreach ($project in $DependencyProjects) {
        $branch = if (
            [string]::IsNullOrWhiteSpace($project.Branch)
        ) {
            "master"
        }
        else {
            $project.Branch
        }
        Sync-GitRepository `
            -Url $project.Url `
            -LocalPath $project.LocalPath `
            -Branch $branch
    }
    # Get version
    $version = Get-ProjectVersion `
        -RepositoryPath $MainProject.LocalPath
    # Form main project list
    $allProjects = @(
        $MainProject
        $DependencyProjects
    )
    # Copy sources
    Copy-ProjectSources `
        -TargetDirectory $TargetDirectory `
        -Projects $allProjects
    # Save version
    Save-BuildVersion `
        -TargetDirectory $TargetDirectory `
        -Version $version
    Write-Host "Sync finished." `
        -ForegroundColor Green
}

function FetchSources {
    Set-Location $BUILDDIR
    Sync-ProjectsAndBuildVersion `
    -TargetDirectory $BASEDIST `
    -MainProject $mainRepo `
    -DependencyProjects $dependencies

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
    $env:PATH += ";$QTDIR;$QTDIR\bin;"
    if ($USENINJA -eq "ON")
    {
        $env:PATH += "$NINJAPATH;"
    }
    else
    {
        $env:PATH += "$JOMPATH;"
    }
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
                   "-DBUNDLED_KEYCHAIN=-ON",
                   "-DVERBOSE_PROGRAM_NAME=ON")
    if ($ISWEBKIT -eq "basic") { $CMAKE_FLAGS += "-DTRANSLATIONS_DIR=`"$LANGSPATH\translations`"" }
    if ($USEDEBUG -eq "ON") { $CMAKE_FLAGS += "-DCMAKE_BUILD_TYPE=Debug" }
    $CMAKE_FLAGS += "-DPRODUCTION=$USEPRODUCTION"
    $ARGSLIST = @($CMAKE_BTYPE,
                "-S", $PSIPATH,
                "-B", $BDIR)
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
    & $CMAKEBIN --build $BDIR --target all --parallel $CPUCOUNT
    "$($script:BTYPE) =$ISWEBKIT compiled" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }

    if ($USEPRODUCTION -eq "ON") {
        & $CMAKEBIN --build $BDIR --target prepare-bin --target prepare-bin-libs --target windeploy
        if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    }
    & $CMAKEBIN --build $BDIR --target install
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

    $CMAKE_FLAGS = @("-DONLY_PLUGINS=ON",
                   "-DBUILD_DEV_PLUGINS=ON",
                   "-DBUILD_PLUGINS=-omemoplugin",
                   "-DBUILD_PSIMEDIA=$BUILD_PSIMEDIA",
                   "-DPKG_CONFIG_EXECUTABLE=`"$PKG_CONFIG_EXECUTABLE`"")
    if ($USEDEBUG -eq "ON") { $CMAKE_FLAGS += "-DCMAKE_BUILD_TYPE=Debug" }
    $ARGSLIST = @($CMAKE_BTYPE,
                "-S", $BASEDIST,
                "-B", $BDIR)
    foreach ($element in $DEFAULT_CMAKE_FLAGS) {
        $ARGSLIST += "$element"
    }
    foreach ($element in $CMAKE_FLAGS) {
        $ARGSLIST += "$element"
    }
    & $CMAKEBIN $ARGSLIST
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    "cmake with flags $CMAKE_FLAGS finished" | Out-File -Append -Encoding UTF8 -FilePath $BUILDLOG
    & $CMAKEBIN --build $BDIR --target all --parallel $CPUCOUNT
    if ($USEPAUSES -eq "ON") { Read-Host "Press Enter to continue..." }
    & $CMAKEBIN --build $BDIR --target install
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
