set QMAKESPEC=win32-g++
::begin
::set /p ARCHTYPE=Do you want to build qca x86_64 version [y/n(default)]:%=%
::@echo _

set /p BINTYPE=Do you want to build qca debug version [y/n(default)]:%=%
@echo _ 

set ROOTDIR=%CD%

set QTDIR=E:\Qt\Qt5\5.5\msvc2013_64
set MSVCDIR=E:\MSVC2013\VC\amd64
set ARCH=win64

if /i "%BINTYPE%"=="y" (
set BTYPE=Debug
) else (
set BTYPE=Release
)

set CMAKEDIR=C:\build\cmake\i386
set OPENSSL_ROOT_DIR=C:\build\newsdk\win64\openssl
set OPENSSL_INCLUDE_DIR=C:\build\newsdk\win64\openssl\include\openssl
set ZLIBDIR=C:\build\newsdk\win64\zlib
set GNUPGDIR=C:\build\GnuPG
set PATH=%QTDIR%\;%QTDIR%\bin;%MSVCDIR%;%MSVCDIR%\bin;%CMAKEDIR%\bin;%OPENSSL_ROOT_DIR%;%OPENSSL_ROOT_DIR%\bin;%ZLIBDIR%;%ZLIBDIR%\bin;%GNUPGDIR%
set CMAKE_ARCH="Visual Studio 12 2013 Win64"
CALL E:\MSVC2013\VC\bin\amd64\vcvars64.bat

set PREFIX=C:\build\newsdk\win64\qca
set BDIR=%ROOTDIR%\_builds
if exist %BDIR% rmdir /S /Q %BDIR%


cmake -H. -B_builds -DCMAKE_BUILD_TYPE=%BTYPE% -G %CMAKE_ARCH% -DCMAKE_INSTALL_PREFIX=%PREFIX% -DBUILD_PLUGINS=ossl;gnupg -DBUILD_TOOLS=OFF -DBUILD_TESTS=OFF
pause
cmake --build _builds --config %BTYPE%
pause
cmake --build _builds --target install --config %BTYPE%
pause
@goto exit

:exit
pause
