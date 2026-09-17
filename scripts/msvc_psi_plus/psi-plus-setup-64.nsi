Unicode true
RequestExecutionLevel user

!include MUI2.nsh
!include LogicLib.nsh
!include x64.nsh
!include nsDialogs.nsh
!include WinVer.nsh
!include StrFunc.nsh
${StrStr}

; Build-time overrides, for example:
; makensis /DWORK_DIR=C:\build\Installer /DRES_DIR=C:\build\resources psi-plus-setup-64.nsi
!ifndef WORK_DIR
  !define WORK_DIR "C:\build\Installer"
!endif
!ifndef RES_DIR
  !define RES_DIR "C:\build\resources"
!endif
!ifndef PSI_SRC_DIR
  !define PSI_SRC_DIR "C:\build\psi"
!endif
!ifndef COMMON_DIR
  !define COMMON_DIR "common"
!endif
!ifndef PLUGINS_DIR
  !define PLUGINS_DIR "plugins"
!endif

!define APP_ID "{751DD547-F64F-44D8-8304-5643E71D409B}"
!define VC_URL "https://aka.ms/vs/17/release/vc_redist.x64.exe"
!define VC_MAJOR 14
!define VC_MINOR 42
!define VC_BUILD 34433
!define STATE_FILE "$TEMP\psi-plus-upgrade.ini"
!define STATE_SECTION "PreviousInstallation"
!define INSTALL_REGKEY "Software\Psi+\Installer"
!define UNINSTALL_REGKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_ID}_is1"

!getdllversion "${WORK_DIR}\psi-plus.exe" APP_VERSION

Name "Psi+"
Caption "Psi+ x64 Setup"
OutFile "${WORK_DIR}\out\psi-plus-${APP_VERSION}-x64-setup.exe"
InstallDir "$LOCALAPPDATA\Psi-plus"
InstallDirRegKey HKCU "${UNINSTALL_REGKEY}" "InstallLocation"
InstallDirRegKey HKLM "${UNINSTALL_REGKEY}" "InstallLocation"
BrandingText "Psi+ Project"
ShowInstDetails show
ShowUninstDetails show

VIProductVersion "${APP_VERSION}"
VIAddVersionKey /LANG=1033 "ProductName" "Psi+"
VIAddVersionKey /LANG=1033 "CompanyName" "Psi+ Project"
VIAddVersionKey /LANG=1033 "FileDescription" "Psi+ x64 installer"
VIAddVersionKey /LANG=1033 "LegalCopyright" "© 2008-2026 Psi+ Project"

!define MUI_ICON "${PSI_SRC_DIR}\win32\app-plus.ico"
!define MUI_UNICON "${PSI_SRC_DIR}\win32\app-plus.ico"
!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Start Psi+"
!define MUI_FINISHPAGE_RUN_FUNCTION LaunchPsi

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${PSI_SRC_DIR}\COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Page custom VCRedistPageCreate VCRedistPageLeave
Page custom SummaryPageCreate SummaryPageLeave
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE English
!insertmacro MUI_LANGUAGE Russian
!insertmacro MUI_LANGUAGE French
!insertmacro MUI_LANGUAGE Ukrainian

Var PreviousUninstaller
Var PreviousVersion
Var PreviousInstallDir
Var PreviousHive
Var NeedVCRedist
Var VCRedistChecked
Var VCRedistDownloaded
Var SelectedExecutable
Var hVCRedistLabel
Var hSummary
Var hSummaryText

; Every file remains wildcard-based. The script only defines selectable groups.
Section /o "Psi+" SEC_BIN64
  SectionIn 1 2 3
  SetOutPath "$INSTDIR"
  File "${WORK_DIR}\psi-plus.exe"
SectionEnd

Section /o "Psi+ WebEngine" SEC_BIN64W
  SectionIn 1 2 3
  SetOutPath "$INSTDIR"
  File "${WORK_DIR}\psi-plus-webengine.exe"
  SetOutPath "$INSTDIR"
  File /r "${WORK_DIR}\webengine64\*.*"
SectionEnd

Section "Psi+ common files" SEC_COMMON
  SectionIn 1 2 3 RO
  SetOutPath "$INSTDIR\certs"
  File /nonfatal /r "${PSI_SRC_DIR}\certs\*.*"
  SetOutPath "$INSTDIR\iconsets\roster"
  File /nonfatal "${WORK_DIR}\iconsets\roster\*.jisp"
  SetOutPath "$INSTDIR\sound"
  File /nonfatal /r "${WORK_DIR}\sound\*.*"
  SetOutPath "$INSTDIR"
  File /nonfatal /r /x iconsets /x sound /x "skins\mac" "${RES_DIR}\*.*"
  File /nonfatal "${WORK_DIR}\client_icons.txt"
  File /nonfatal /oname=CHANGELOG.TXT "${PSI_SRC_DIR}\CHANGELOG"
  SetOutPath "$INSTDIR\translations"
  File /nonfatal /r "${WORK_DIR}\translations\*.*"
  SetOutPath "$INSTDIR"
  File /nonfatal /r "${WORK_DIR}\lib64\*.*"
  File /nonfatal "${PSI_SRC_DIR}\COPYING"
  File "${COMMON_DIR}\README.txt"
SectionEnd

; This section runs first and is deliberately hidden from the component tree.
Section "-Visual C++ Redistributable" SEC_VC
  ${If} $NeedVCRedist == 1
    Call DownloadAndInstallVCRedist
  ${EndIf}
SectionEnd

; NSIS parameterized blocks must be declared with !macro, not !define.
!macro PLUGIN_SECTION name text
Section /o "${text}" SEC_PLUGIN_${name}
  SetOutPath "$INSTDIR\plugins"
  File /nonfatal "${PLUGINS_DIR}\${name}.dll"
SectionEnd
!macroend

SectionGroup /e "Plugins" SEC_PLUGINS
!insertmacro PLUGIN_SECTION attentionplugin "Attention plugin"
!insertmacro PLUGIN_SECTION autoreplyplugin "Auto Reply plugin"
!insertmacro PLUGIN_SECTION battleshipgameplugin "Battleship Game plugin"
!insertmacro PLUGIN_SECTION birthdayreminderplugin "Birthday Reminder plugin"
!insertmacro PLUGIN_SECTION chessplugin "Chess plugin"
!insertmacro PLUGIN_SECTION cleanerplugin "Cleaner plugin"
!insertmacro PLUGIN_SECTION clientswitcherplugin "Client Switcher plugin"
!insertmacro PLUGIN_SECTION conferenceloggerplugin "Conference Logger plugin"
!insertmacro PLUGIN_SECTION contentdownloaderplugin "Content Downloader plugin"
!insertmacro PLUGIN_SECTION enummessagesplugin "Enum Messages plugin"
!insertmacro PLUGIN_SECTION extendedmenuplugin "Extended Menu plugin"
!insertmacro PLUGIN_SECTION extendedoptionsplugin "Extended Options plugin"
!insertmacro PLUGIN_SECTION gomokugameplugin "Gomoku Game plugin"
!insertmacro PLUGIN_SECTION historykeeperplugin "History Keeper plugin"
!insertmacro PLUGIN_SECTION imageplugin "Image plugin"
!insertmacro PLUGIN_SECTION imagepreviewplugin "Image Preview plugin"
!insertmacro PLUGIN_SECTION jabberdiskplugin "Jabber Disk plugin"
!insertmacro PLUGIN_SECTION juickplugin "Juick plugin"
!insertmacro PLUGIN_SECTION messagefilterplugin "Message Filter plugin"
!insertmacro PLUGIN_SECTION noughtsandcrossesplugin "Noughts And Crosses plugin"
!insertmacro PLUGIN_SECTION openpgpplugin "OpenPGP plugin"
!insertmacro PLUGIN_SECTION otrplugin "OTR plugin"
!insertmacro PLUGIN_SECTION pepchangenotifyplugin "PEP Change Notify plugin"
!insertmacro PLUGIN_SECTION qipxstatusesplugin "Qip X-Statuses plugin"
!insertmacro PLUGIN_SECTION redirectorplugin "Redirector plugin"
!insertmacro PLUGIN_SECTION skinsplugin "Skins plugin"
!insertmacro PLUGIN_SECTION stopspamplugin "Stop Spam plugin"
!insertmacro PLUGIN_SECTION storagenotesplugin "Storage Notes plugin"
!insertmacro PLUGIN_SECTION translateplugin "Translate plugin"
!insertmacro PLUGIN_SECTION videostatusplugin "Video Status Changer plugin"
!insertmacro PLUGIN_SECTION watcherplugin "Watcher plugin"
SectionGroupEnd

!macro DICT_SECTION name text
Section /o "${text}" SEC_DICT_${name}
  SetOutPath "$INSTDIR\myspell\dicts"
  File /nonfatal "${COMMON_DIR}\myspell\dicts\${name}.*"
SectionEnd
!macroend

SectionGroup /e "Spell check dictionaries" SEC_DICTS
!insertmacro DICT_SECTION af_ZA "Afrikaans"
!insertmacro DICT_SECTION an_ES "Aragonese"
!insertmacro DICT_SECTION ar "عربي"
!insertmacro DICT_SECTION be_BY "Беларуская мова"
!insertmacro DICT_SECTION bg_BG "Български"
!insertmacro DICT_SECTION bn_BD "Bengali"
!insertmacro DICT_SECTION br_FR "Brezhoneg"
!insertmacro DICT_SECTION bs_BA "Bosanski"
!insertmacro DICT_SECTION cs_CZ "Čeština"
!insertmacro DICT_SECTION da_DK "Dansk"
!insertmacro DICT_SECTION de_AT_frami "Deutsch (Österreich)"
!insertmacro DICT_SECTION de_CH_frami "Deutsch (Schweiz)"
!insertmacro DICT_SECTION de_DE_frami "Deutsch (Deutschland)"
!insertmacro DICT_SECTION el_GR "Ελληνικά"
!insertmacro DICT_SECTION en_AU "English (Australia)"
!insertmacro DICT_SECTION en_CA "English (Canada)"
!insertmacro DICT_SECTION en_GB "English (United Kingdom)"
!insertmacro DICT_SECTION en_US "English (United States)"
!insertmacro DICT_SECTION en_ZA "English (South Africa)"
!insertmacro DICT_SECTION es_ANY "Español"
!insertmacro DICT_SECTION et_EE "Eesti keel"
!insertmacro DICT_SECTION fr "Français"
!insertmacro DICT_SECTION gl_ES "Galego"
!insertmacro DICT_SECTION gu_IN "Gujarati (India)"
!insertmacro DICT_SECTION he_IL "עברית"
!insertmacro DICT_SECTION hi_IN "Hindi"
!insertmacro DICT_SECTION hr_HR "Hrvatski"
!insertmacro DICT_SECTION hu_HU "Magyar"
Section /o "Íslenska" SEC_DICT_is_dic
  SetOutPath "$INSTDIR\myspell\dicts"
  File /nonfatal /oname=is.aff "${COMMON_DIR}\myspell\dicts\is.aff"
  File /nonfatal /oname=is.dic "${COMMON_DIR}\myspell\dicts\is.dic"
SectionEnd
!insertmacro DICT_SECTION it_IT "Italiano"
!insertmacro DICT_SECTION kmr_Latn "Kurdish (Latin)"
!insertmacro DICT_SECTION lo_LA "Lao"
Section /o "Lietuvių kalba" SEC_DICT_lt
  SetOutPath "$INSTDIR\myspell\dicts"
  File /nonfatal "${COMMON_DIR}\myspell\dicts\lt.*"
SectionEnd
!insertmacro DICT_SECTION lv_LV "Latviešu"
!insertmacro DICT_SECTION nb_NO "Norsk (Bokmål)"
!insertmacro DICT_SECTION ne_NP "Nepali"
!insertmacro DICT_SECTION nl_NL "Nederlands"
!insertmacro DICT_SECTION nn_NO "Norsk (Nynorsk)"
!insertmacro DICT_SECTION oc_FR "Occitan"
!insertmacro DICT_SECTION pl_PL "Polski"
!insertmacro DICT_SECTION pt_BR "Português do Brasil"
!insertmacro DICT_SECTION pt_PT "Português de Portugal"
!insertmacro DICT_SECTION ro_RO "Română"
!insertmacro DICT_SECTION ru_RU "Русский"
!insertmacro DICT_SECTION si_LK "Sinhala"
!insertmacro DICT_SECTION sk_SK "Slovenčina"
!insertmacro DICT_SECTION sl_SI "Slovenščina"
Section /o "Српски (Latin)" SEC_DICT_sr_Latn
  SetOutPath "$INSTDIR\myspell\dicts"
  File /nonfatal "${COMMON_DIR}\myspell\dicts\sr-Latn.*"
SectionEnd
!insertmacro DICT_SECTION sr "Српски"
!insertmacro DICT_SECTION sv_FI "Svenska (Finland)"
!insertmacro DICT_SECTION sv_SE "Svenska (Sverige)"
!insertmacro DICT_SECTION sw_TZ "Kiswahili (Tanzania)"
!insertmacro DICT_SECTION te_IN "Telugu (India)"
!insertmacro DICT_SECTION th_TH "Thai"
!insertmacro DICT_SECTION uk_UA "Українська"
!insertmacro DICT_SECTION vi_VN "Tiếng Việt"
SectionGroupEnd

Section "Create desktop shortcut" SEC_DESKTOP
  ${If} ${SectionIsSelected} ${SEC_BIN64}
    CreateShortCut "$DESKTOP\Psi+ (x64).lnk" "$INSTDIR\psi-plus.exe" "" "$INSTDIR\psi-plus.exe"
  ${ElseIf} ${SectionIsSelected} ${SEC_BIN64W}
    CreateShortCut "$DESKTOP\Psi+ (x64).lnk" "$INSTDIR\psi-plus-webengine.exe" "" "$INSTDIR\psi-plus-webengine.exe"
  ${EndIf}
SectionEnd

Section /o "Create uninstall icon in Program Group" SEC_UNINSTALL_ICON
  CreateDirectory "$SMPROGRAMS\Psi+ (x64)"
  CreateShortCut "$SMPROGRAMS\Psi+ (x64)\Uninstall Psi+.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Create XMPP registry entry" SEC_XMPP
  ${If} ${SectionIsSelected} ${SEC_BIN64}
    WriteRegStr HKCR "xmpp" "" "URL:XMPP Protocol"
    WriteRegStr HKCR "xmpp" "URL Protocol" ""
    WriteRegStr HKCR "xmpp\DefaultIcon" "" "$INSTDIR\psi-plus.exe"
    WriteRegStr HKCR "xmpp\shell\open\command" "" '"$INSTDIR\psi-plus.exe" "--uri=%1"'
  ${ElseIf} ${SectionIsSelected} ${SEC_BIN64W}
    WriteRegStr HKCR "xmpp" "" "URL:XMPP Protocol"
    WriteRegStr HKCR "xmpp" "URL Protocol" ""
    WriteRegStr HKCR "xmpp\DefaultIcon" "" "$INSTDIR\psi-plus-webengine.exe"
    WriteRegStr HKCR "xmpp\shell\open\command" "" '"$INSTDIR\psi-plus-webengine.exe" "--uri=%1"'
  ${EndIf}
SectionEnd

Section "-Installer metadata"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKCU "${UNINSTALL_REGKEY}" "DisplayName" "Psi+ ${APP_VERSION}"
  WriteRegStr HKCU "${UNINSTALL_REGKEY}" "DisplayVersion" "${APP_VERSION}"
  WriteRegStr HKCU "${UNINSTALL_REGKEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "${UNINSTALL_REGKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKCU "${UNINSTALL_REGKEY}" "DisplayIcon" "$INSTDIR\psi-plus.exe"
  WriteRegStr HKCU "${UNINSTALL_REGKEY}" "Publisher" "Psi+ Project"
  WriteRegDWORD HKCU "${UNINSTALL_REGKEY}" "NoModify" 1
  WriteRegDWORD HKCU "${UNINSTALL_REGKEY}" "NoRepair" 1
  Call SaveInstallerState
SectionEnd

Function .onInit
  ${IfNot} ${RunningX64}
    MessageBox MB_ICONSTOP "This installer requires 64-bit Windows."
    Abort
  ${EndIf}
  Delete "${STATE_FILE}"
  Call FindPreviousInstallation
  ${If} $PreviousUninstaller != ""
    Call HandlePreviousInstallation
  ${EndIf}
FunctionEnd

Function FindPreviousInstallation
  StrCpy $PreviousUninstaller ""
  StrCpy $PreviousHive ""
  SetRegView 64
  ReadRegStr $PreviousUninstaller HKCU "${UNINSTALL_REGKEY}" "UninstallString"
  StrCpy $PreviousHive HKCU
  ${If} $PreviousUninstaller == ""
    ReadRegStr $PreviousUninstaller HKLM "${UNINSTALL_REGKEY}" "UninstallString"
    StrCpy $PreviousHive HKLM
  ${EndIf}
  SetRegView 32
  ${If} $PreviousUninstaller == ""
    ReadRegStr $PreviousUninstaller HKCU "${UNINSTALL_REGKEY}" "UninstallString"
    StrCpy $PreviousHive HKCU
  ${EndIf}
  ${If} $PreviousUninstaller == ""
    ReadRegStr $PreviousUninstaller HKLM "${UNINSTALL_REGKEY}" "UninstallString"
    StrCpy $PreviousHive HKLM
  ${EndIf}
  SetRegView 64
  ${If} $PreviousUninstaller != ""
    ReadRegStr $PreviousVersion ${PreviousHive} "${UNINSTALL_REGKEY}" "DisplayVersion"
    ReadRegStr $PreviousInstallDir ${PreviousHive} "${UNINSTALL_REGKEY}" "InstallLocation"
  ${EndIf}
FunctionEnd

Function HandlePreviousInstallation
  MessageBox MB_ICONINFORMATION|MB_OKCANCEL "An installed Psi+ version $PreviousVersion was found.$\r$\n$\r$\nClick OK to continue. The existing version will be removed before installing the new version." IDOK ContinueUpgrade
  Abort
ContinueUpgrade:
  Call CheckRunningPsi
  Call SavePreviousState
  ExecWait '$PreviousUninstaller /SILENT /NORESTART /SUPPRESSMSGBOXES' $0
  ${If} $0 != 0
    MessageBox MB_ICONSTOP "The previous Psi+ version could not be removed."
    Abort
  ${EndIf}
  Call RestorePreviousState
FunctionEnd

Function CheckRunningPsi
  nsProcess::FindProcess "psi-plus.exe"
  Pop $0
  ${If} $0 == 0
    MessageBox MB_ICONQUESTION|MB_YESNO "Psi+ is currently running. Close it before continuing?" IDYES ClosePsi
    Abort
  ${EndIf}
  nsProcess::FindProcess "psi-plus-webengine.exe"
  Pop $0
  ${If} $0 == 0
    MessageBox MB_ICONQUESTION|MB_YESNO "Psi+ WebEngine is currently running. Close it before continuing?" IDYES ClosePsi
    Abort
  ${EndIf}
  Return
ClosePsi:
  ExecWait 'taskkill.exe /F /IM psi-plus.exe' $0
  ExecWait 'taskkill.exe /F /IM psi-plus-webengine.exe' $0
  Sleep 500
  nsProcess::FindProcess "psi-plus.exe"
  Pop $0
  ${If} $0 == 0
    MessageBox MB_ICONSTOP "Psi+ is still running. Close it manually and restart the installer."
    Abort
  ${EndIf}
FunctionEnd

Function SavePreviousState
  SetRegView 64
  ReadRegStr $0 ${PreviousHive} "${UNINSTALL_REGKEY}" "InstallLocation"
  WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "InstallDir" "$0"
  ReadRegStr $1 ${PreviousHive} "${UNINSTALL_REGKEY}" "Inno Setup: Selected Components"
  WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "SelectedComponents" "$1"
  ReadRegStr $1 ${PreviousHive} "${UNINSTALL_REGKEY}" "Inno Setup: Selected Tasks"
  WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "SelectedTasks" "$1"
FunctionEnd

Function RestorePreviousState
  ReadINIStr $0 "${STATE_FILE}" "${STATE_SECTION}" "InstallDir"
  ${If} $0 != ""
    StrCpy $INSTDIR $0
  ${EndIf}
  Call RestoreComponentState
FunctionEnd

!macro RESTORE_COMPONENT name
  ${StrStr} $0 $PreviousComponents "${name}"
  ${If} $0 != ""
    SectionSetFlags ${SEC_PLUGIN_${name}} ${SECTION_SELECTED}
  ${EndIf}
!macroend

!macro RESTORE_DICT name
  ${StrStr} $0 $PreviousComponents "dicts\\${name}"
  ${If} $0 != ""
    SectionSetFlags ${SEC_DICT_${name}} ${SECTION_SELECTED}
  ${EndIf}
!macroend

Function RestoreComponentState
  ReadINIStr $PreviousComponents "${STATE_FILE}" "${STATE_SECTION}" "SelectedComponents"
  ${StrStr} $1 $PreviousComponents "bin64w"
  ${If} $1 != ""
    SectionSetFlags ${SEC_BIN64W} ${SECTION_SELECTED}
    SectionSetFlags ${SEC_BIN64} 0
  ${Else}
    ${StrStr} $1 $PreviousComponents "bin64"
    ${If} $1 != ""
      SectionSetFlags ${SEC_BIN64} ${SECTION_SELECTED}
      SectionSetFlags ${SEC_BIN64W} 0
    ${EndIf}
  ${EndIf}
  !insertmacro RESTORE_COMPONENT attentionplugin
  !insertmacro RESTORE_COMPONENT autoreplyplugin
  !insertmacro RESTORE_COMPONENT openpgpplugin
  !insertmacro RESTORE_COMPONENT otrplugin
  !insertmacro RESTORE_COMPONENT translateplugin
  !insertmacro RESTORE_COMPONENT videostatusplugin
  !insertmacro RESTORE_DICT en_US
  !insertmacro RESTORE_DICT ru_RU
  !insertmacro RESTORE_DICT uk_UA
FunctionEnd

Function VCRedistPageCreate
  nsDialogs::Create 1018
  Pop $0
  ${If} $0 == error
    Abort
  ${EndIf}
  ${NSD_CreateLabel} 0 0 100% 100u "Checking Microsoft Visual C++ Redistributable for x64..."
  Pop $hVCRedistLabel
  Call CheckVCRedist
  ${If} $NeedVCRedist == 1
    ${NSD_SetText} $hVCRedistLabel "Microsoft Visual C++ Redistributable x64 is missing or older than the required version (14.42.34433.0).$\r$\n$\r$\nThe official package will be downloaded from https://aka.ms/vs/17/release/vc_redist.x64.exe"
  ${Else}
    ${NSD_SetText} $hVCRedistLabel "Microsoft Visual C++ Redistributable x64 is already installed with a suitable version. No download is required."
  ${EndIf}
  nsDialogs::Show
FunctionEnd

Function VCRedistPageLeave
  StrCpy $VCRedistChecked 1
FunctionEnd

Function CheckVCRedist
  StrCpy $NeedVCRedist 1
  SetRegView 64
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Major"
  ${If} ${Errors}
    Return
  ${EndIf}
  ReadRegDWORD $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Minor"
  ReadRegDWORD $2 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Bld"
  ReadRegDWORD $3 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "RBld"
  ${If} $0 > ${VC_MAJOR}
    StrCpy $NeedVCRedist 0
  ${ElseIf} $0 == ${VC_MAJOR}
    ${If} $1 > ${VC_MINOR}
      StrCpy $NeedVCRedist 0
    ${ElseIf} $1 == ${VC_MINOR}
      ${If} $2 >= ${VC_BUILD}
        StrCpy $NeedVCRedist 0
      ${EndIf}
    ${EndIf}
  ${EndIf}
FunctionEnd

Function SummaryPageCreate
  nsDialogs::Create 1018
  Pop $0
  ${NSD_CreateLabel} 0 0 100% 30u "The following will be installed:"
  Pop $hSummary
  ${NSD_CreateText} 0 35u 100% 150u "Psi+ ${APP_VERSION}$\r$\nInstall directory: $INSTDIR$\r$\nSelected components will be copied from wildcard-based build directories.$\r$\n$\r$\nThe selected Psi+ components, plugins and Hunspell dictionaries will be installed."
  Pop $hSummaryText
  ${If} $NeedVCRedist == 1
    ${NSD_SetText} $hSummaryText "Psi+ ${APP_VERSION}$\r$\nInstall directory: $INSTDIR$\r$\n$\r$\nMicrosoft Visual C++ Redistributable x64 will be downloaded from the official Microsoft URL and installed.$\r$\n$\r$\nThe selected files, plugins and Hunspell dictionaries will then be installed."
  ${EndIf}
  nsDialogs::Show
FunctionEnd

Function SummaryPageLeave
FunctionEnd

Function DownloadAndInstallVCRedist
  DetailPrint "Downloading Microsoft Visual C++ Redistributable..."
  inetc::get /caption "Downloading Microsoft Visual C++ Redistributable" /canceltext "Cancel" "${VC_URL}" "$PLUGINSDIR\vc_redist.x64.exe"
  Pop $0
  ${If} $0 != "OK"
    MessageBox MB_ICONSTOP "Could not download Microsoft Visual C++ Redistributable.$\r$\n$\r$\nError: $0"
    Abort
  ${EndIf}
  DetailPrint "Installing Microsoft Visual C++ Redistributable..."
  ExecWait '"$PLUGINSDIR\vc_redist.x64.exe" /install /quiet /norestart' $0
  ${If} $0 != 0
    MessageBox MB_ICONSTOP "Microsoft Visual C++ Redistributable installation failed. Exit code: $0"
    Abort
  ${EndIf}
FunctionEnd

Function SaveInstallerState
  WriteRegStr HKCU "${INSTALL_REGKEY}" "InstallDir" "$INSTDIR"
  WriteRegStr HKCU "${INSTALL_REGKEY}" "Executable" "$SelectedExecutable"
FunctionEnd

Function .onSelChange
  ${If} ${SectionIsSelected} ${SEC_BIN64}
    SectionSetFlags ${SEC_BIN64W} 0
    StrCpy $SelectedExecutable "psi-plus.exe"
  ${ElseIf} ${SectionIsSelected} ${SEC_BIN64W}
    SectionSetFlags ${SEC_BIN64} 0
    StrCpy $SelectedExecutable "psi-plus-webengine.exe"
  ${EndIf}
FunctionEnd

Function .onInstSuccess
  Delete "${STATE_FILE}"
FunctionEnd
Function .onInstFailed
  Delete "${STATE_FILE}"
FunctionEnd
Function .onUserAbort
  Delete "${STATE_FILE}"
FunctionEnd

Function LaunchPsi
  ${If} $SelectedExecutable == "psi-plus-webengine.exe"
    ExecShell open "$INSTDIR\psi-plus-webengine.exe"
  ${Else}
    ExecShell open "$INSTDIR\psi-plus.exe"
  ${EndIf}
FunctionEnd

Function un.onInit
  nsProcess::FindProcess "psi-plus.exe"
  Pop $0
  ${If} $0 == 0
    ExecWait 'taskkill.exe /F /IM psi-plus.exe' $1
  ${EndIf}
  nsProcess::FindProcess "psi-plus-webengine.exe"
  Pop $0
  ${If} $0 == 0
    ExecWait 'taskkill.exe /F /IM psi-plus-webengine.exe' $1
  ${EndIf}
FunctionEnd

Section "Uninstall"
  Delete "$DESKTOP\Psi+ (x64).lnk"
  Delete "$SMPROGRAMS\Psi+ (x64)\Psi+ (x64).lnk"
  Delete "$SMPROGRAMS\Psi+ (x64)\Uninstall Psi+.lnk"
  RMDir "$SMPROGRAMS\Psi+ (x64)"

  ; Delete only registry data created by this installer.
  ; Application settings and QtKeychain data are intentionally preserved.
  DeleteRegKey HKCR "xmpp"
  DeleteRegKey HKCU "${UNINSTALL_REGKEY}"
  DeleteRegKey HKLM "${UNINSTALL_REGKEY}"
  DeleteRegKey HKCU "${INSTALL_REGKEY}"

  RMDir /r "$INSTDIR"
SectionEnd
