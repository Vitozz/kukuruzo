set /p ARCHTYPE=Do you want to build amd64 version [y/n(default)]:%=%
@echo _

set ROOTDIR=%CD%

set SEVENZIP="C:\Program Files\7-Zip\7z.exe"
set CMAKEDIR="C:\build\cmake\i386"

if /i "%ARCHTYPE%"=="y" (
set VSDIR=E:\MSVC2013\VC\amd64
set ARCH=win64
set CMAKE_ARCH="Visual Studio 12 2013 Win64"
CALL E:\MSVC2013\VC\bin\amd64\vcvars64.bat
) else (
set VSDIR=E:\MSVC2013\VC
set ARCH=win32
set CMAKE_ARCH="Visual Studio 12 2013"
CALL E:\MSVC2013\VC\bin\vcvars32.bat
)

set PATH=%PATH%;%SEVENZIP%;%CMAKEDIR%;%CMAKEDIR%\bin;%VSDIR%;%VSDIR%\bin

set PREFIX=C:\build\newsdk\%ARCH%\libtidy

set BDIR=%ROOTDIR%\_builds

if exist %BDIR% rmdir /S /Q %BDIR%
::mkdir %BDIR% && cd %BDIR%

cmake --build _builds --target clean

cmake -H. -B_builds -DCMAKE_BUILD_TYPE=Release -G %CMAKE_ARCH% -DCMAKE_INSTALL_PREFIX=%PREFIX%
pause
::cmake --build . --target tidy
cmake --build _builds --config Release
pause
cmake --build _builds --target install --config Release
pause

cd %ROOTDIR%

@goto exit

:exit
pause
