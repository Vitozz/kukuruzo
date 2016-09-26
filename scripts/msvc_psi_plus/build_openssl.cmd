set /p ARCHTYPE=Do you want to build amd64 version [y/n(default)]:%=%
@echo _

set ROOTDIR=%CD%

set OPENSSL_VERSION=1.0.2i
set SEVENZIP="C:\Program Files\7-Zip\7z.exe"

set NASM_DIR="E:\SOFT\nasm"
set CMAKEDIR="C:\build\cmake\i386"
set PERLDIR="E:\Perl64\bin"

if /i "%ARCHTYPE%"=="y" (
set VSDIR=E:\MSVC2013\VC\amd64
set ARCH=win64
set VCSPEC=VC-WIN64A
CALL E:\MSVC2013\VC\bin\amd64\vcvars64.bat
set PREP_CMD=ms\do_win64a
) else (
set VSDIR=E:\MSVC2013\VC
set ARCH=win32
set VCSPEC=VC-WIN32
CALL E:\MSVC2013\VC\bin\vcvars32.bat
set PREP_CMD=ms\do_nasm
)

set PATH=%PATH%;%SEVENZIP%;%NASM_DIR%;%PERLDIR%;%VSDIR%;%VSDIR%\bin

nmake -f ms\ntdll.mak clean

set PREFIX=C:\build\newsdk\%ARCH%\openssl

perl Configure %VCSPEC% --prefix=%PREFIX%
@echo Running %PREP_CMD%
CALL %PREP_CMD%
pause
@echo Running %nmake%
pause
nmake -f ms\ntdll.mak
nmake -f ms\ntdll.mak install
pause

@goto exit

:exit
pause
