Unicode true
RequestExecutionLevel admin
ManifestSupportedOS win7
SetCompressor /SOLID lzma
SetDatablockOptimize on
SetOverwrite on

!include MUI2.nsh
!include LogicLib.nsh
!include WinVer.nsh

!ifndef WORK_DIR
  !define WORK_DIR "C:\build"
!endif
!ifndef SDK_DIR
  !define SDK_DIR "${WORK_DIR}\sdk"
!endif
!ifndef OUTPUT_DIR
  !define OUTPUT_DIR "${WORK_DIR}\Output"
!endif

!define PRODUCT_NAME "Psi+ SDK Qt6 (MSVC2022)"
!define PRODUCT_VERSION "52.0"
!define PRODUCT_PUBLISHER "Psi+ Development Team"
!define PRODUCT_WEB_SITE "http://sourceforge.net/projects/psiplus/"
!define PRODUCT_ID "{C604E814-BA3A-4B60-B1AB-433D35414844}"
!define PRODUCT_REGKEY "Software\Psi+ Development Team\PsiSDK_MSVC"
!define UNINSTALL_REGKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}"
!define ENVIRONMENT_REGKEY "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
!define ENV_NAME "PSI_SDK_MSVC_WIN64"
!define PATH_NAME "Path"
!define INSTALL_DEFAULT "C:\PsiSDK_MSVC"
!define PATH_ENTRIES "$INSTDIR\bin;$INSTDIR\lib;$INSTDIR\include"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
Caption "${PRODUCT_NAME} Setup"
OutFile "${OUTPUT_DIR}\psisdk-msvc2022-setup-${PRODUCT_VERSION}.exe"
InstallDir "${INSTALL_DEFAULT}"
InstallDirRegKey HKLM "${PRODUCT_REGKEY}" "InstallLocation"
BrandingText "${PRODUCT_PUBLISHER}"
ShowInstDetails show
ShowUnInstDetails show

VIProductVersion "52.0.0.0"
VIAddVersionKey /LANG=1033 "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=1033 "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=1033 "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=1033 "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=1033 "FileDescription" "${PRODUCT_NAME} installer"
VIAddVersionKey /LANG=1033 "LegalCopyright" "${PRODUCT_PUBLISHER}"

!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_TITLE "Установка завершена"
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME} установлен в $INSTDIR. Для применения переменных окружения рекомендуется перезагрузить Windows."

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Ukrainian"

Var PreviousUninstaller
Var PreviousVersion
Var PreviousInstallDir
Var PreviousHive
Var PathOldValue
Var PathNewValue

Function .onInit
  SetRegView 64
  SectionSetFlags ${SEC_HUNSPELL} ${SF_SELECTED}
  SectionSetFlags ${SEC_OPENSSL} ${SF_SELECTED}
  SectionSetFlags ${SEC_PROTOBUF} ${SF_SELECTED}
  SectionSetFlags ${SEC_QCA} ${SF_SELECTED}
  SectionSetFlags ${SEC_QTKEYCHAIN} ${SF_SELECTED}
  SectionSetFlags ${SEC_SRTP} ${SF_SELECTED}
  SectionSetFlags ${SEC_ZLIB} ${SF_SELECTED}
  SectionSetFlags ${SEC_ENV} ${SF_SELECTED}
  SectionSetFlags ${SEC_PATH} ${SF_SELECTED}
  Call FindPreviousInstallation
  ${If} $PreviousUninstaller != ""
    Call HandlePreviousInstallation
  ${EndIf}
FunctionEnd

Function FindPreviousInstallation
  StrCpy $PreviousUninstaller ""
  StrCpy $PreviousVersion ""
  StrCpy $PreviousInstallDir ""
  StrCpy $PreviousHive ""
  ReadRegStr $PreviousUninstaller HKLM "${UNINSTALL_REGKEY}" "UninstallString"
  ${If} $PreviousUninstaller != ""
    StrCpy $PreviousHive "HKLM"
    ReadRegStr $PreviousVersion HKLM "${UNINSTALL_REGKEY}" "DisplayVersion"
    ReadRegStr $PreviousInstallDir HKLM "${UNINSTALL_REGKEY}" "InstallLocation"
  ${EndIf}
FunctionEnd

Function HandlePreviousInstallation
  MessageBox MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2 "Обнаружена установленная версия ${PRODUCT_NAME} ($PreviousVersion).$$
$$
Перед установкой новой версии необходимо удалить предыдущую. Удалить её автоматически сейчас?" IDYES RemovePrevious IDCANCEL CancelUpgrade
CancelUpgrade:
  Abort
RemovePrevious:
  ${If} ${FileExists} "$PreviousUninstaller"
    DetailPrint "Удаление предыдущей версии..."
    ExecWait '"$PreviousUninstaller" /S' $0
    ${If} $0 != 0
      MessageBox MB_ICONSTOP|MB_OK "Не удалось удалить предыдущую версию (код $0). Установка отменена."
      Abort
    ${EndIf}
  ${Else}
    MessageBox MB_ICONSTOP|MB_OK "Файл деинсталлятора предыдущей версии не найден. Установка отменена."
    Abort
  ${EndIf}
FunctionEnd

SectionGroup /e "Компоненты SDK"
Section "Hunspell" SEC_HUNSPELL
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\hunspell\*.*"
SectionEnd
Section "OpenSSL" SEC_OPENSSL
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\openssl\*.*"
SectionEnd
Section "Protobuf" SEC_PROTOBUF
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\protobuf\*.*"
SectionEnd
Section "QCA" SEC_QCA
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\qca\*.*"
SectionEnd
Section "QtKeychain" SEC_QTKEYCHAIN
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\qtkeychain\*.*"
SectionEnd
Section "SRTP" SEC_SRTP
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\srtp\*.*"
SectionEnd
Section "zlib" SEC_ZLIB
  SetOutPath "$INSTDIR"
  File /r /nonfatal "${SDK_DIR}\zlib\*.*"
SectionEnd
SectionGroupEnd

Section /o "Добавить PSI_SDK_MSVC_WIN64 в переменные среды" SEC_ENV
SectionEnd
Section /o "Добавить bin, lib и include в PATH" SEC_PATH
SectionEnd

Section -PostInstall
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_REGKEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${PRODUCT_REGKEY}" "Version" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_REGKEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${UNINSTALL_REGKEY}" "DisplayIcon" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKLM "${UNINSTALL_REGKEY}" "NoModify" 1
  WriteRegDWORD HKLM "${UNINSTALL_REGKEY}" "NoRepair" 1

  SectionGetFlags ${SEC_ENV} $0
  IntOp $0 $0 & ${SF_SELECTED}
  ${If} $0 != 0
    WriteRegStr HKLM "${ENVIRONMENT_REGKEY}" "${ENV_NAME}" "$INSTDIR"
    WriteRegStr HKLM "${PRODUCT_REGKEY}" "EnvironmentVariable" "${ENV_NAME}"
    WriteRegStr HKLM "${UNINSTALL_REGKEY}" "EnvironmentVariable" "${ENV_NAME}"
  ${EndIf}

  SectionGetFlags ${SEC_PATH} $0
  IntOp $0 $0 & ${SF_SELECTED}
  ${If} $0 != 0
    ReadRegStr $PathOldValue HKLM "${ENVIRONMENT_REGKEY}" "${PATH_NAME}"
    WriteRegStr HKLM "${PRODUCT_REGKEY}" "OriginalPath" "$PathOldValue"
    WriteRegStr HKLM "${UNINSTALL_REGKEY}" "OriginalPath" "$PathOldValue"
    ${If} $PathOldValue == ""
      StrCpy $PathNewValue "${PATH_ENTRIES}"
    ${Else}
      StrCpy $PathNewValue "$PathOldValue;${PATH_ENTRIES}"
    ${EndIf}
    WriteRegExpandStr HKLM "${ENVIRONMENT_REGKEY}" "${PATH_NAME}" "$PathNewValue"
    WriteRegStr HKLM "${PRODUCT_REGKEY}" "PathAdded" "${PATH_ENTRIES}"
    WriteRegStr HKLM "${UNINSTALL_REGKEY}" "PathAdded" "${PATH_ENTRIES}"
  ${EndIf}
  SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd

Function un.onInit
  SetRegView 64
FunctionEnd

Section Uninstall
  SetRegView 64
  ReadRegStr $0 HKLM "${UNINSTALL_REGKEY}" "EnvironmentVariable"
  ${If} $0 != ""
    DeleteRegValue HKLM "${ENVIRONMENT_REGKEY}" "$0"
  ${EndIf}
  ReadRegStr $PathOldValue HKLM "${PRODUCT_REGKEY}" "OriginalPath"
  ReadRegStr $0 HKLM "${UNINSTALL_REGKEY}" "PathAdded"
  ${If} $0 != ""
    WriteRegExpandStr HKLM "${ENVIRONMENT_REGKEY}" "${PATH_NAME}" "$PathOldValue"
  ${EndIf}
  DeleteRegKey HKLM "${UNINSTALL_REGKEY}"
  DeleteRegKey HKLM "${PRODUCT_REGKEY}"
  RMDir /r "$INSTDIR"
  SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd
