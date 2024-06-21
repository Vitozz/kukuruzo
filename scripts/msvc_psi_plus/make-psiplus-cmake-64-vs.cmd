@ECHO off
::This is a psi-plus build script
ECHO:PSI-PLUS BUILD SCRIPT
ECHO.
set /p FETCH="Do you want to download/update psi+ sources [y(default)/n]"
ECHO.

::======ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ========
::КОЛ-ВО CPU В СИСТЕМЕ
set /a CPUCOUNT=%NUMBER_OF_PROCESSORS%+1
::СОБРАТЬ ПСИ+ С DEBUG ИНФОРМАЦИЕЙ OFF/ON
set USEDEBUG=OFF
::СБОРКА ДЛЯ ПРОДАКШН
set USEPRODUCTION=OFF
::ПАУЗЫ ПРИ СБОРКЕ
set USEPAUSES=OFF
::BUILD PSIMEDIA
set BUILD_PSIMEDIA=ON

::======АБСОЛЮТНЫЕ ПУТИ=========
::ОБЩИЙ КАТАЛОГ СБОРКИ
set BUILDDIR=C:\build
::КАТАЛОГ С QT
set QTPREFIX=C:\Qt
::ВЕРСИЯ QT
set QTVER=6
::ПУТЬ К СКРИПТУ vcvarsall
set VCVARS=C:\MSVC2019\Base\VC\Auxiliary\Build\vcvarsall.bat
::INNO SETUP COMPILER
set ISS_COMPIL="C:\Program Files (x86)\Inno Setup 6\Compil32.exe"
::GIT
set GITBIN_DIR=%BUILDDIR%\Git
set GIT_EXECUTABLE=%GITBIN_DIR%\bin\git.exe
::PKG_CONFIG
set PKG_CONFIG_EXECUTABLE=C:/gstreamer/1.0/msvc_x86_64/bin/pkg-config.exe
::CCACHE
set CCACHE_BIN_DIR=%BUILDDIR%\ccache-4.9.1-windows-x86_64

::======ОТНОСИТЕЛЬНЫЕ ПУТИ=========
set PARENT_DIR=%CD%
::ЛОГ-ФАЙЛ
set LOGDIR=%PARENT_DIR%\logs
set BUILDLOG=%LOGDIR%\build.log
set ERRLOGPREF=%LOGDIR%\psi-plus-vs-errors
::КАТАЛОГ ИСХОДНИКОВ
set PSIDIST=%BUILDDIR%
::КАТАЛОГ ИСХОДНИКОВ ПСИ
set PSIPATH=%PSIDIST%\psi
::КАТАЛОГ ИСХОДНИКОВ ПСИ+
set BASEDIST=%PSIDIST%\psi-build-src
::КАТАЛОГ ПЕРЕВОДОВ ПСИ+
set LANGSPATH=%PSIDIST%\psi-plus-l10n
::КАТАЛОГ РЕСУРСОВ
set RESPATH=%PSIDIST%\resources
::КАТАЛОГ ПЛАГИНОВ
set PLUGSPATH=%PSIDIST%\plugins
::КАТАЛОГ PSIMEDIA
set MEDIAPATH=%PSIDIST%\psimedia
::РАБОЧИЙ КАТАЛОГ ПОДГОТОВКИ ИСХОДНИКОВ
set WORKDIR=%PSIDIST%
::УТИЛИТЫ НЕОБХОДИМЫЕ ДЛЯ ПОДГОТОВКИ И СБОРКИ
set GITBIN=%GIT_EXECUTABLE%
set PATCHBIN=%GIT_DIR%\usr\bin\patch.exe
set CMAKEDIR=%QTPREFIX%\Tools\Cmake_64
set NINJAPATH=%QTPREFIX%\Tools\Ninja
set NINJA=%NINJAPATH%\ninja.exe

::====НЕ МЕНЯЙТЕ ЭТО ПОЖАЛУЙСТА=====
set binTemplate=win64
set webkitTemplate=webengine
set qtTemplate=qt%QTVER%

::======ПЕРЕМЕННЫЕ win64=======
set ARCHNAME=win64
::======ПЕРЕМЕННЫЕ Qt win64=====
set QTDIRQT6=%QTPREFIX%\6.7.1\msvc2019_64
ECHO:Logging started>%BUILDLOG%
ECHO:_>>%BUILDLOG%

::======НАЧАЛО СЕКЦИИ ПОДГОТОВКИ=======
IF "%FETCH%"=="n" (
	@goto preparesrc
)
@goto fetchsources

:fetchsources
	cd %BUILDDIR%
	::СКРИПТ СКАЧИВАЕТ ИСХОДНЫЕ ФАЙЛЫ И ПОДГОТАВЛИВАЕТ ИХ К СБОРКЕ В КАТАЛОГЕ psi-build-src
	%CMAKEDIR%\bin\cmake -DBUILD_DIR="C:/build" -P %BUILDDIR%\Prepare-psi.cmake
	@goto preparesrc

:preparesrc
	IF NOT EXIST %BASEDIST% (
		ECHO:%BASEDIST% not found. Please run script again and download psi+ sources"
		@goto exitlabel
	)

	cd %WORKDIR%
	IF "%USEPAUSES%"=="ON" pause

	SET RETURN=finish
	@goto selection
::======КОНЕЦ СЕКЦИИ ПОДГОТОВКИ=======

::======СЕКЦИЯ ВЫБОРА======
:selection
	ECHO:Choose start point^: 
	ECHO:"-1--Build All"
	ECHO:"-2--Start from Qt%QTVER%"
	ECHO:"-3--Start from Qt%QTVER% Webkit"
	ECHO:"-4--Build plugins"
	ECHO:"-5--Exit"
	set /p BUILD_VARIANT="Select 1-5 and Press Enter: "
	IF "%BUILD_VARIANT%"=="1" @goto mainloop
	IF "%BUILD_VARIANT%"=="2" @goto buildqt
	IF "%BUILD_VARIANT%"=="3" @goto buildqtw
	IF "%BUILD_VARIANT%"=="4" @goto buildplugs
	IF "%BUILD_VARIANT%"=="5" @goto exitlabel

::======НАЧАЛО СЕКЦИИ СБОРКИ======
::===ЦИКЛ===
:mainloop
	ECHO:"Normal Qt%QTVER% version"
	SET ISWEBKIT=basic
	set BPLUGS=OFF
	SET RETURN=buildqtw
	@goto setQtvars

	:buildqt
		ECHO:"Normal Qt%QTVER% version"
		SET ISWEBKIT=basic
		set BPLUGS=OFF
		SET RETURN=buildqtw
		@goto setQtvars

	:buildqtw
		ECHO:"Webengine Qt%QTVER% version"
		SET ISWEBKIT=webengine
		set BPLUGS=OFF
		SET RETURN=buildplugs
		@goto setQtvars

	:buildplugs
		ECHO:"Plugins"
		set BPLUGS=ON
		SET RETURN=crsetup
		@goto setQtvars

	:crsetup
		SET RETURN=finish
		@goto createsetup

	:finish
		@goto exitlabel

::======ПЕРЕМЕННЫЕ QT=====
:setQtvars
	set ARCH=%ARCHNAME%
	IF "%QTVER%"=="6" set QTDIR=%QTDIRQT6%
	set BTYPE=W64
	ECHO:* Building %BTYPE%-^=%ISWEBKIT%-%qtTemplate% psi+ binary *>>%BUILDLOG%
	ECHO:_>>%BUILDLOG%
	ECHO:%BTYPE% variables^:>>%BUILDLOG%
	ECHO:QT%QTVER% in %QTDIR%>>%BUILDLOG%
	@goto setcommonvars

::======ОБЩИЕ ПЕРЕМЕННЫЕ======
:setcommonvars
	set WINVARS=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0\;
	set PATH=%WINVARS%
	CALL %VCVARS% amd64
	set PATH=%PATH%;%QTDIR%;%QTDIR%\bin;%NINJAPATH%;%PSI_SDK_MSVC_WIN64%;%PSI_SDK_MSVC_WIN64%\bin;%PSI_SDK_MSVC_WIN64%\lib;%PSI_SDK_MSVC_WIN64%\include;%CCACHE_BIN_DIR%;
	ECHO:%PATH%>>%BUILDLOG%

	::DEFAULT CMAKE FLAGS
	set DEFAULT_CMAKE_FLAGS=-DPSI_PLUS=ON -DGIT_EXECUTABLE=%GIT_EXECUTABLE% -DUSE_CCACHE=ON -DQT_DEFAULT_MAJOR_VERSION=%QTVER% -DCMAKE_INSTALL_PREFIX="%PSIDIST%\Installer"
	IF NOT "%BPLUGS%"=="ON" @goto makeit
	IF "%BPLUGS%"=="ON" @goto makeplugins

::======СБОРКА======
:makeit
	cd %WORKDIR%
	set BDIR=%PSIDIST%\psi-plus_build
	if exist %BDIR% rmdir /S /Q %BDIR%
	mkdir %BDIR% && cd %BDIR%

	set CMAKE_FLAGS=%DEFAULT_CMAKE_FLAGS% -DCHAT_TYPE=%ISWEBKIT% -DBUNDLED_IRIS_ALL=ON -DVERBOSE_PROGRAM_NAME=ON
	IF "%ISWEBKIT%"=="basic" set CMAKE_FLAGS=%CMAKE_FLAGS% -DTRANSLATIONS_DIR=%LANGSPATH%\translations
	IF "%USEDEBUG%"=="ON" set CMAKE_FLAGS=%CMAKE_FLAGS% -DCMAKE_BUILD_TYPE=Debug
	set CMAKE_FLAGS=%CMAKE_FLAGS% -DPRODUCTION=%USEPRODUCTION%

	%CMAKEDIR%\bin\cmake -G "Ninja" %CMAKE_FLAGS% -S %PSIPATH%

	IF "%USEPAUSES%"=="ON" pause
	ECHO:cmake with flags %CMAKE_FLAGS% finished>>%BUILDLOG%
	ECHO:%BTYPE% ^=%ISWEBKIT% configured>>%BUILDLOG%

	%CMAKEDIR%\bin\cmake --build %BDIR% --target all --parallel %CPUCOUNT%
	ECHO:%BTYPE% ^=%ISWEBKIT% compiled>>%BUILDLOG%
	IF "%USEPAUSES%"=="ON" pause

	IF "%USEPRODUCTION%"=="ON" (
		%CMAKEDIR%\bin\cmake --build %BDIR% --target prepare-bin --target prepare-bin-libs --target windeploy
		IF "%USEPAUSES%"=="ON" pause
	)

	%CMAKEDIR%\bin\cmake --build %BDIR% --target install
	IF "%USEPAUSES%"=="ON" pause

	cd %WORKDIR%
	::CLEANUP
	if exist %BDIR% rmdir /S /Q %BDIR%
	@goto %RETURN%

:makeplugins
	cd %WORKDIR%
	set BDIR=%PSIDIST%\psi-plugins_build
	if exist %BDIR% rmdir /S /Q %BDIR%
	mkdir %BDIR% && cd %BDIR%

	set CMAKE_FLAGS=%DEFAULT_CMAKE_FLAGS% -DONLY_PLUGINS=ON -DBUILD_DEV_PLUGINS=%BPLUGS% -DBUNDLED_OMEMO_C_ALL=ON -DBUILD_PSIMEDIA=%BUILD_PSIMEDIA% -DPKG_CONFIG_EXECUTABLE=%PKG_CONFIG_EXECUTABLE%
	IF "%USEDEBUG%"=="ON" set CMAKE_FLAGS=%CMAKE_FLAGS% -DCMAKE_BUILD_TYPE=Debug
	%CMAKEDIR%\bin\cmake -G "Ninja" %CMAKE_FLAGS% -S %BASEDIST%
	IF "%USEPAUSES%"=="ON" pause
	ECHO:cmake with flags %CMAKE_FLAGS% finished>>%BUILDLOG%
	%CMAKEDIR%\bin\cmake --build %BDIR% --target all --parallel %CPUCOUNT%
	IF "%USEPAUSES%"=="ON" pause
	%CMAKEDIR%\bin\cmake --build %BDIR% --target install
	IF "%USEPAUSES%"=="ON" pause
	cd %WORKDIR%
	::CLEANUP
	if exist %BDIR% rmdir /S /Q %BDIR%
	@goto %RETURN%
::======КОНЕЦ СЕКЦИИ СБОРКИ=======

:createsetup
	set /p ANS6="Do you want to create psi-plus installer [y(default)/n]"
	IF /I "%ANS6%"=="n" @goto exitlabel
	set INSTALLERDIR=%BUILDDIR%\Installer
	%ISS_COMPIL% /cc %INSTALLERDIR%\psi-plus-setup-64.iss
	@goto %RETURN%

::======ВЫХОД=======
:exitlabel
	ECHO:EXITING...
	pause
	EXIT 0

