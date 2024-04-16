@ECHO off
::This is a psi-plus build script
ECHO:PSI-PLUS BUILD SCRIPT
ECHO.
set /p FETCH="Do you want to download/update psi+ sources [y/n(default)]"
ECHO.

::======ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ========
::КОЛ-ВО CPU В СИСТЕМЕ +1
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
set SEVENZIP="C:\Program files\7-Zip"
::КАТАЛОГ ГДЕ БУДУТ СОЗДАНЫ 7z АРХИВЫ
set BUILDDIR=C:\build
::КАТАЛОГ С QT5
set QTPREFIX=C:\Qt
::ВЕРСИЯ QT
set QTVER=6
::ПУТИ К ОБМЕННИКАМ
set GDRIVEPATH=E:\SOFT\GoogleDrive\psi-plus
::ПУТЬ К СКРИПТУ vcvarsall
set VCVARS=C:\MSVC2019\Base\VC\Auxiliary\Build\vcvarsall.bat
::INNO SETUP COMPILER
set ISS_COMPIL="C:\Program Files (x86)\Inno Setup 6\Compil32.exe"
::GIT
set GITBIN_DIR=C:\build\Git
set GIT_EXECUTABLE=%GITBIN_DIR%\bin\git.exe
::PKG_CONFIG
set PKG_CONFIG_EXECUTABLE=C:/gstreamer/1.0/msvc_x86_64/bin/pkg-config.exe

::======ОТНОСИТЕЛЬНЫЕ ПУТИ=========
set PARENT_DIR=%CD%
::ЛОГ-ФАЙЛ
set LOGDIR=%PARENT_DIR%\logs
set BUILDLOG=%LOGDIR%\build.log
set ERRLOGPREF=%LOGDIR%\psi-plus-vs-errors
::КАТАЛОГ ИСХОДНИКОВ
set PSIDIST=%BUILDDIR%
::КАТАЛОГ ИСХОДНИКОВ ПСИ+
set BASEDIST=%PSIDIST%\psi-plus-snapshots
::КАТАЛОГ ПЕРЕВОДОВ ПСИ+
set LANGSPATH=%PSIDIST%\psi-plus-l10n
::КАТАЛОГ РЕСУРСОВ
set RESPATH=%PSIDIST%\resources
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
::ТЕКУЩАЯ ДАТА
set BYEAR=%date:~6,4%
set BMONTH=%date:~3,2%
set BDAY=%date:~0,2%
set BHOURS=%time:~0,2%
if "%BHOURS:~0,1%" == " " set BHOURS=0%BHOURS:~1,1%
set BMINS=%time:~3,2%
set PSI_DATE=%BYEAR%-%BMONTH%-%BDAY%
set COMPILATION_DATE=%BYEAR%%BMONTH%%BDAY%
set COMPILATION_TIME=%BHOURS%%BMINS%
set PACKAGE_VERSION=%COMPILATION_DATE%_%COMPILATION_TIME%
::НАСТРОЙКИ СЖАТИЯ 7z
set SEVENZFLAGS=-mx=9 -m0=LZMA -mmt=on

::======ПЕРЕМЕННЫЕ win64=======
set ARCH32=win64
::======ПЕРЕМЕННЫЕ Qt win64=====
set QTDIRQT5=%QTPREFIX%\5.15.2\msvc2019_64
set QTDIRQT6=%QTPREFIX%\6.7.0\msvc2019_64
ECHO:Logging started>%BUILDLOG%
ECHO:_>>%BUILDLOG%

::======НАЧАЛО СЕКЦИИ ПОДГОТОВКИ=======
IF /I NOT "%FETCH%"=="y" @goto preparesrc
cd %PSIDIST%
ECHO:Updating psi sources...
SET FETCHDIR=%BASEDIST%
SET PSIPLUSPROJ=https://github.com/psi-plus
SET PSIPROJ=https://github.com/psi-im
SET FETCHURL=%PSIPLUSPROJ%/psi-plus-snapshots.git
SET RETURN=fetchtrs
@goto fetch

:fetchtrs
	ECHO:Updating translations...
	SET FETCHDIR=%LANGSPATH%
	SET FETCHURL=%PSIPLUSPROJ%/psi-plus-l10n.git
	SET RETURN=fetchres
	@goto fetch

::МЕТКА СКАЧИВАНИЯ РЕСУРСОВ
:fetchres
	ECHO:Updating resources...
	SET FETCHDIR=%RESPATH%
	SET FETCHURL=%PSIPLUSPROJ%/resources.git
	SET RETURN=preparesrc
	@goto fetch

:preparesrc
	IF NOT EXIST %BASEDIST% (
		ECHO:%BASEDIST% not found. Please run script again and download psi+ sources"
		@goto exitlabel
	)
	cd %PSIDIST%

	set /p fullversion=< %BASEDIST%\version
	ECHO:%fullversion%
	set psiver=%fullversion:~,3%
	ECHO:%psiver%>%WORKDIR%\psiver
	set PACKAGE_VERSION=%psiver%.%PACKAGE_VERSION%
	ECHO:"PACKAGE_VERSION=%PACKAGE_VERSION%"

	cd %WORKDIR%
	IF "%USEPAUSES%"=="ON" pause

	SET RETURN=finish
	@goto selection

	:fetch
		cd %PSIDIST%
		IF NOT EXIST "%FETCHDIR%" (
			%GITBIN% clone %FETCHURL% %FETCHDIR%
			cd %FETCHDIR%
			%GITBIN% submodule init
			%GITBIN% submodule update
		) ELSE (
			cd %FETCHDIR%
			%GITBIN% reset --hard
			%GITBIN% clean -xfd
			%GITBIN% pull
			%GITBIN% submodule init
			%GITBIN% submodule update
		)
		@goto %RETURN%
::======КОНЕЦ СЕКЦИИ ПОДГОТОВКИ=======

::======СЕКЦИЯ ВЫБОРА======
:selection
	ECHO:Choose start point^: 
	ECHO:"-1--Build All"
	ECHO:"-2--Start from Qt%QTVER%"
	ECHO:"-3--Start from Qt%QTVER% Webkit"
	ECHO:"-4--Exit"
	set /p BUILD_VARIANT="Select 1-4 and Press Enter: "
	IF "%BUILD_VARIANT%"=="1" @goto mainloop
	IF "%BUILD_VARIANT%"=="2" @goto buildqt
	IF "%BUILD_VARIANT%"=="3" @goto buildqtw
	IF "%BUILD_VARIANT%"=="4" @goto exitlabel

::======НАЧАЛО СЕКЦИИ СБОРКИ И АРХИВАЦИИ======
::===ЦИКЛ===
:mainloop
	ECHO:"Normal Qt%QTVER% version"
	SET ISWEBKIT=basic
	set BPLUGS=ON
	SET RETURN=buildqtw
	@goto setQtvars

	:buildqt
		ECHO:"Normal Qt%QTVER% version"
		SET ISWEBKIT=basic
		set BPLUGS=ON
		SET RETURN=buildqtw
		@goto setQtvars

	:buildqtw
		ECHO:"Webengine Qt%QTVER% version"
		SET ISWEBKIT=webengine
		set BPLUGS=OFF
		SET RETURN=gdrive
		@goto setQtvars

	:gdrive
		SET RETURN=crsetup
		SET DISKPATH=%GDRIVEPATH%
		@goto drivecopy

	:crsetup
		SET RETURN=finish
		@goto createsetup

	:finish
		@goto exitlabel

::======ПЕРЕМЕННЫЕ QT=====
:setQtvars
	set ARCH=%ARCH32%
	set MINGWDIR=%MINGWQT5%
	IF "%QTVER%"=="5" set QTDIR=%QTDIRQT5%
	IF "%QTVER%"=="6" set QTDIR=%QTDIRQT6%
	set BTYPE=W64
	::set SIGNAL_C_ROOT=%PSIDEPSPREFIX%
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
	set PATH=%PATH%;%QTDIR%;%QTDIR%\bin;%NINJAPATH%;%PSI_SDK_MSVC_WIN64%;
	ECHO:%PATH%>>%BUILDLOG%

	@goto makeit

::======АРХИВАЦИЯ=======
:archivate
	IF NOT "%ISWEBKIT%"=="basic" SET wbkPref=-%webkitTemplate%
	IF "%ISWEBKIT%"=="basic" SET wbkPref=""
	SET sysType=-%binTemplate%
	SET qtPref=-%qtTemplate%
	set INARCH=%PSIDIST%\psi-plus_build\psi\psi-plus%wbkPref%.exe
	IF NOT EXIST "%INARCH%" (
		ECHO:ERROR NO %INARCH% BINARY FOUND
		ECHO:ERROR NO %INARCH% BINARY FOUND>>%BUILDLOG%
		@goto exitlabel
	)
	set EXITARCH="%BUILDDIR%\psi-plus-%PACKAGE_VERSION%%wbkPref%%sysType%%qtPref%.7z"
	%SEVENZIP%\7z.exe a -t7z %EXITARCH% %INARCH% %SEVENZFLAGS%
	ECHO:Archive file %EXITARCH% created>>%BUILDLOG%
	::Переводы
	set TINARCH=%PSIDIST%\psi-plus_build\psi\translations
	set TEXITARCH="%BUILDDIR%\psi-plus-%PACKAGE_VERSION%-translations%qtPref%.7z"
	%SEVENZIP%\7z.exe a -t7z %TEXITARCH% %TINARCH% %SEVENZFLAGS%
	ECHO:Archive file %TEXITATCH% created>>%BUILDLOG%
	::Плагины
	set PINARCH=%PSIDIST%\psi-plus_build\psi\plugins
	set PEXITARCH="%BUILDDIR%\psi-plus-%PACKAGE_VERSION%-plugins%sysType%%qtPref%.7z"
	IF "%BPLUGS%"=="ON" %SEVENZIP%\7z.exe a -t7z %PEXITARCH% %PINARCH% %SEVENZFLAGS%
	IF "%BPLUGS%"=="ON" ECHO:Archive file %PEXITARCH% created>>%BUILDLOG%
	ECHO:_>>%BUILDLOG%
	set PRODINARCH=%PSIDIST%\psi-plus_build\psi
	set PRODEXITARCH="%BUILDDIR%\psi-plus-%PACKAGE_VERSION%-all-%sysType%%qtPref%.7z"
	IF "%USEPRODUCTION%"=="ON" %SEVENZIP%\7z.exe a -t7z %PRODEXITARCH% %PRODINARCH% %SEVENZFLAGS%
	IF "%USEPRODUCTION%"=="ON" ECHO:Archive file %PRODEXITARCH% created>>%BUILDLOG%
	IF "%USEPAUSES%"=="ON" pause
	@goto %RETURN%

::======СБОРКА======
:makeit
	cd %WORKDIR%
	set BDIR=%PSIDIST%\psi-plus_build
	if exist %BDIR% rmdir /S /Q %BDIR%
	mkdir %BDIR% && cd %BDIR%
	
	set CMAKE_FLAGS=-DCHAT_TYPE=%ISWEBKIT% -DPSI_PLUS=ON -DBUNDLED_IRIS_ALL=ON -DGIT_EXECUTABLE=%GIT_EXECUTABLE% -DTRANSLATIONS_DIR=%LANGSPATH%/translations
	IF "%USEDEBUG%"=="ON" set CMAKE_FLAGS=%CMAKE_FLAGS% -DCMAKE_BUILD_TYPE=Debug
	IF "%BPLUGS%"=="ON" set CMAKE_FLAGS=%CMAKE_FLAGS% -DBUILD_DEV_PLUGINS=%BPLUGS% -DBUILD_PSIMEDIA=%BUILD_PSIMEDIA% -DPKG_CONFIG_EXECUTABLE=%PKG_CONFIG_EXECUTABLE%
	set CMAKE_FLAGS=%CMAKE_FLAGS% -DENABLE_PLUGINS=%BPLUGS%
	set CMAKE_FLAGS=%CMAKE_FLAGS% -DPRODUCTION=%USEPRODUCTION% -DQT_DEFAULT_MAJOR_VERSION=%QTVER% -DVERBOSE_PROGRAM_NAME=ON
	
	%CMAKEDIR%\bin\cmake -G "Ninja" %CMAKE_FLAGS% %BASEDIST%
	::%CMAKEDIR%\bin\cmake -G "NMake Makefiles JOM" %CMAKE_FLAGS% %BASEDIST%
	
	IF "%USEPAUSES%"=="ON" pause
	ECHO:cmake with flags %CMAKE_FLAGS% finished>>%BUILDLOG%
	ECHO:%BTYPE% ^=%ISWEBKIT% configured>>%BUILDLOG%
	
	::%CMAKEDIR%\bin\cmake --build %BDIR% --target all -- -j15 2> %ERRLOGPREF%-W-%ISWEBKIT%.log
	%CMAKEDIR%\bin\cmake --build %BDIR% --target all 2> %ERRLOGPREF%-W-%ISWEBKIT%.log
	ECHO:%BTYPE% ^=%ISWEBKIT% compiled>>%BUILDLOG%
	IF "%USEPAUSES%"=="ON" pause
	
	IF "%USEPRODUCTION%"=="ON" %CMAKEDIR%\bin\cmake --build %BDIR% --target prepare-bin
	IF "%USEPAUSES%"=="ON" pause
	IF "%USEPRODUCTION%"=="ON" %CMAKEDIR%\bin\cmake --build %BDIR% --target prepare-bin-libs
	IF "%USEPAUSES%"=="ON" pause
	IF "%USEPRODUCTION%"=="ON" %CMAKEDIR%\bin\cmake --build %BDIR% --target windeploy
	IF "%USEPAUSES%"=="ON" pause
	
	cd %WORKDIR%
	if "%BPLUGS%"=="ON" DEL %BDIR%\psi\plugins\*.exp
	if "%BPLUGS%"=="ON" DEL %BDIR%\psi\plugins\*.manifest
	if "%BPLUGS%"=="ON" DEL %BDIR%\psi\plugins\*.lib
	@goto archivate
::======КОНЕЦ СЕКЦИИ СБОРКИ И АРХИВАЦИИ=======

::======КОПИРОВАНИЕ НА ДИСК======
:drivecopy
	::set /p ANS4="Do you want to copy build data to Drive [y/n(default)]"
	::IF /I NOT "%ANS4%"=="y" @goto installercopy
	::set QT5_YD=%DISKPATH%\%bin32Template%\Qt5-64
	::IF NOT EXIST "%QT5_YD%" mkdir %QT5_YD%
	::IF NOT EXIST "%QT5_YD%\plugins" mkdir %QT5_YD%\plugins
	::IF NOT EXIST "%QT5_YD%\%webkitTemplate%" mkdir %QT5_YD%\%webkitTemplate%
	::IF NOT EXIST "%QT5_YD%\%nHistoryTemplate%" mkdir %QT5_YD%\%nHistoryTemplate%
	::IF NOT EXIST "%QT5_YD%\%nHistoryTemplate%\%webkitTemplate%" mkdir %QT5_YD%\%nHistoryTemplate%\%webkitTemplate%
	::IF NOT EXIST "%DISKPATH%\translations" mkdir %DISKPATH%\translations
	
	::PLUGINS
	::COPY /Y %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-plugins-%bin32Template%-%qt5Template%.7z %QT5_YD%\plugins
	::TRANSLATIONS
	::COPY /Y %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-translations-%qt5Template%.7z %DISKPATH%\translations
	::PSI
	::COPY /Y %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-%bin32Template%-%qt5Template%.7z %QT5_YD%
	::PSI-WEBKIT
	::COPY /Y %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-%webkitTemplate%-%bin32Template%-%qt5Template%.7z %QT5_YD%\%webkitTemplate%
	::PSI-NEW-HISTORY
	::COPY /Y %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-%nHistoryTemplate%-%bin32Template%-%qt5Template%.7z %QT5_YD%\%nHistoryTemplate%
	::PSI-NEW-HISTORY-WEBKIT
	::COPY /Y %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-%webkitTemplate%-%nHistoryTemplate%-%bin32Template%-%qt5Template%.7z %QT5_YD%\%nHistoryTemplate%\%webkitTemplate%
	@goto installercopy

::======ПОДГОТОВКА ФАЙЛОВ ДЛЯ ИНСТАЛЛЕРА=======
:installercopy
	set /p ANS5="Do you want to prepare psi-plus installer files [y/n(default)]"
	IF /I NOT "%ANS5%"=="y" @goto exitlabel
	set INSTALLERDIR=%BUILDDIR%\Installer
	IF NOT EXIST "%INSTALLERDIR%" mkdir %INSTALLERDIR%
	IF NOT EXIST "%INSTALLERDIR%\bin64" mkdir %INSTALLERDIR%\bin64
	::IF NOT EXIST "%INSTALLERDIR%\bin64" mkdir %INSTALLERDIR%\bin64
	IF NOT EXIST "%INSTALLERDIR%\plugins64" mkdir %INSTALLERDIR%\plugins64
	IF NOT EXIST "%INSTALLERDIR%\translations64" mkdir %INSTALLERDIR%\translations64
	::PSI-PLUS
	%SEVENZIP%\7z.exe e -aoa %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-%binTemplate%-%qtTemplate%.7z -o%INSTALLERDIR%\bin64
	::PSI-PLUS-WEBKIT
	%SEVENZIP%\7z.exe e -aoa %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-%webkitTemplate%-%binTemplate%-%qtTemplate%.7z -o%INSTALLERDIR%\bin64
	::PLUGINS32
	%SEVENZIP%\7z.exe e -aoa %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-plugins-%binTemplate%-%qtTemplate%.7z -o%INSTALLERDIR%\plugins64 *.dll -r
	::TRANSLATIONS
	%SEVENZIP%\7z.exe e -aoa %BUILDDIR%\psi-plus-%PACKAGE_VERSION%-translations-%qtTemplate%.7z -o%INSTALLERDIR%\translations64 *.qm -r
	@goto %RETURN%

:createsetup
	set /p ANS6="Do you want to create psi-plus installer [y/n(default)]"
	IF /I NOT "%ANS6%"=="y" @goto exitlabel
	set INSTALLERDIR=%BUILDDIR%\Installer
	%ISS_COMPIL% /cc %INSTALLERDIR%\psi-plus-setup-64.iss
	@goto %RETURN%

::======ВЫХОД=======
:exitlabel
	ECHO:EXITING...
	pause
	EXIT 0

