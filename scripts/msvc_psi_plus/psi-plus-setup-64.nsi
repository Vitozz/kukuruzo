Unicode true
RequestExecutionLevel user

!include MUI2.nsh
!include LogicLib.nsh
!include x64.nsh
!include nsDialogs.nsh

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
!define STATE_FILE "$APPDATA\Psi+\installer-state.ini"
!define STATE_SECTION "PreviousInstallation"
!define UNINSTALL_REGKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_ID}_is1"

!getdllversion "${WORK_DIR}\psi-plus.exe" APP_VERSION_
!define APP_VERSION "${APP_VERSION_1}.${APP_VERSION_2}.${APP_VERSION_3}.${APP_VERSION_4}"

Name "Psi+"
Caption "Psi+ x64 Setup"
OutFile "${WORK_DIR}\out\psi-plus-${APP_VERSION}-x64-setup.exe"
InstallDir "$LOCALAPPDATA\Psi-plus"
InstallDirRegKey HKCU "${UNINSTALL_REGKEY}" "InstallLocation"
BrandingText "Psi+ Project"
ShowInstDetails show
ShowUninstDetails show

VIProductVersion "${APP_VERSION}"
VIAddVersionKey /LANG=1033 "ProductVersion" "${APP_VERSION}"
VIAddVersionKey /LANG=1033 "FileVersion" "${APP_VERSION}"
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

!define MUI_PAGE_HEADER_TEXT "$(STR_VC_PAGE_TITLE)"
!define MUI_PAGE_HEADER_SUBTEXT "$(STR_VC_PAGE_SUBTITLE)"
Page custom VCRedistPageCreate VCRedistPageLeave
!undef MUI_PAGE_HEADER_TEXT
!undef MUI_PAGE_HEADER_SUBTEXT

!define MUI_PAGE_HEADER_TEXT "$(STR_SUMMARY_PAGE_TITLE)"
!define MUI_PAGE_HEADER_SUBTEXT "$(STR_SUMMARY_PAGE_SUBTITLE)"
Page custom SummaryPageCreate SummaryPageLeave
!undef MUI_PAGE_HEADER_TEXT
!undef MUI_PAGE_HEADER_SUBTEXT

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE English
!insertmacro MUI_LANGUAGE Russian
!insertmacro MUI_LANGUAGE French
!insertmacro MUI_LANGUAGE Ukrainian
!insertmacro MUI_LANGUAGE German

!include "lang\psi-plus-l10n.nsh"

Var PreviousUninstaller
Var PreviousVersion
Var PreviousInstallDir
Var PreviousHive
Var NeedVCRedist
Var SelectedExecutable
Var hVCRedistLabel
Var hSummaryText

; Save the selected state for each installer section so it can be restored later.
!macro SAVE_COMPONENT key section
  SectionGetFlags ${section} $0
  IntOp $0 $0 & ${SF_SELECTED}
  ${If} $0 != 0
    WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "Component.${key}" "1"
  ${Else}
    WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "Component.${key}" "0"
  ${EndIf}
!macroend

; Restore a section state from the saved installer-state file.
!macro RESTORE_COMPONENT key section
  ReadINIStr $0 "${STATE_FILE}" "${STATE_SECTION}" "Component.${key}"
  ${If} $0 == "1"
    SectionSetFlags ${section} ${SF_SELECTED}
  ${ElseIf} $0 == "0"
    SectionSetFlags ${section} 0
  ${EndIf}
!macroend

Section /o "Psi+" SEC_BIN64
  SectionIn 1 2 3
  SetOutPath "$INSTDIR"
  File "${WORK_DIR}\psi-plus.exe"
SectionEnd

Section /o "Psi+ WebEngine" SEC_BIN64W
  SectionIn 1 2 3
  SetOutPath "$INSTDIR"
  File "${WORK_DIR}\psi-plus-webengine.exe"
  File /r /x ".*" "${WORK_DIR}\webengine64\*.*"
SectionEnd

Section "Psi+ common files" SEC_COMMON
  SectionIn 1 2 3 RO
  SetOutPath "$INSTDIR\certs"
  File /nonfatal /r /x ".*" "${PSI_SRC_DIR}\certs\*.*"
  SetOutPath "$INSTDIR\iconsets\roster"
  File /nonfatal /x ".*" "${WORK_DIR}\iconsets\roster\*.jisp"
  SetOutPath "$INSTDIR\sound"
  File /nonfatal /r /x ".*" "${WORK_DIR}\sound\*.*"
  SetOutPath "$INSTDIR"
  File /nonfatal /r /x iconsets /x sound /x "skins\mac" /x ".*" "${RES_DIR}\*.*"
  File /nonfatal "${WORK_DIR}\client_icons.txt"
  File /nonfatal /oname=CHANGELOG.TXT "${PSI_SRC_DIR}\CHANGELOG"
  SetOutPath "$INSTDIR\translations"
  File /nonfatal /r /x ".*" "${WORK_DIR}\translations\*.*"
  SetOutPath "$INSTDIR"
  File /nonfatal /r /x ".*" "${WORK_DIR}\lib64\*.*"
  File /nonfatal "${PSI_SRC_DIR}\COPYING"
  File "${COMMON_DIR}\README.txt"
SectionEnd

Section "-Visual C++ Redistributable" SEC_VC
  ${If} $NeedVCRedist == 1
    Call DownloadAndInstallVCRedist
  ${EndIf}
SectionEnd

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
!insertmacro PLUGIN_SECTION mediaplugin "PsiMedia plugin"
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
!insertmacro DICT_SECTION ar "Arabic"
!insertmacro DICT_SECTION be_BY "Belarusian"
!insertmacro DICT_SECTION bg_BG "Bulgarian"
!insertmacro DICT_SECTION bn_BD "Bengali"
!insertmacro DICT_SECTION br_FR "Breton"
!insertmacro DICT_SECTION bs_BA "Bosnian"
!insertmacro DICT_SECTION cs_CZ "Czech"
!insertmacro DICT_SECTION da_DK "Danish"
!insertmacro DICT_SECTION de_AT_frami "German (Austria)"
!insertmacro DICT_SECTION de_CH_frami "German (Switzerland)"
!insertmacro DICT_SECTION de_DE_frami "German (Germany)"
!insertmacro DICT_SECTION el_GR "Greek"
!insertmacro DICT_SECTION en_AU "English (Australia)"
!insertmacro DICT_SECTION en_CA "English (Canada)"
!insertmacro DICT_SECTION en_GB "English (United Kingdom)"
!insertmacro DICT_SECTION en_US "English (United States)"
!insertmacro DICT_SECTION en_ZA "English (South Africa)"
!insertmacro DICT_SECTION es_ANY "Spanish"
!insertmacro DICT_SECTION et_EE "Estonian"
!insertmacro DICT_SECTION fr "French"
!insertmacro DICT_SECTION gl_ES "Galician"
!insertmacro DICT_SECTION gu_IN "Gujarati"
!insertmacro DICT_SECTION he_IL "Hebrew"
!insertmacro DICT_SECTION hi_IN "Hindi"
!insertmacro DICT_SECTION hr_HR "Croatian"
!insertmacro DICT_SECTION hu_HU "Hungarian"
Section /o "Icelandic" SEC_DICT_is_dic
  SetOutPath "$INSTDIR\myspell\dicts"
  File /nonfatal /oname=is.aff "${COMMON_DIR}\myspell\dicts\is.aff"
  File /nonfatal /oname=is.dic "${COMMON_DIR}\myspell\dicts\is.dic"
SectionEnd
!insertmacro DICT_SECTION it_IT "Italian"
!insertmacro DICT_SECTION kmr_Latn "Kurdish (Latin)"
!insertmacro DICT_SECTION lo_LA "Lao"
!insertmacro DICT_SECTION lt "Lithuanian"
!insertmacro DICT_SECTION lv_LV "Latvian"
!insertmacro DICT_SECTION nb_NO "Norwegian (Bokmål)"
!insertmacro DICT_SECTION ne_NP "Nepali"
!insertmacro DICT_SECTION nl_NL "Dutch"
!insertmacro DICT_SECTION nn_NO "Norwegian (Nynorsk)"
!insertmacro DICT_SECTION oc_FR "Occitan"
!insertmacro DICT_SECTION pl_PL "Polish"
!insertmacro DICT_SECTION pt_BR "Portuguese (Brazil)"
!insertmacro DICT_SECTION pt_PT "Portuguese (Portugal)"
!insertmacro DICT_SECTION ro_RO "Romanian"
!insertmacro DICT_SECTION ru_RU "Russian"
!insertmacro DICT_SECTION si_LK "Sinhala"
!insertmacro DICT_SECTION sk_SK "Slovak"
!insertmacro DICT_SECTION sl_SI "Slovenian"
Section /o "Serbian (Latin)" SEC_DICT_sr_Latn
  SetOutPath "$INSTDIR\myspell\dicts"
  File /nonfatal "${COMMON_DIR}\myspell\dicts\sr-Latn.*"
SectionEnd
!insertmacro DICT_SECTION sr "Serbian"
!insertmacro DICT_SECTION sv_FI "Swedish (Finland)"
!insertmacro DICT_SECTION sv_SE "Swedish (Sweden)"
!insertmacro DICT_SECTION sw_TZ "Swahili (Tanzania)"
!insertmacro DICT_SECTION te_IN "Telugu"
!insertmacro DICT_SECTION th_TH "Thai"
!insertmacro DICT_SECTION uk_UA "Ukrainian"
!insertmacro DICT_SECTION vi_VN "Vietnamese"
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

; Select the standard Psi+ binary by default when no saved state exists.
Function SetDefaultBinaryComponent
  SectionSetFlags ${SEC_BIN64} ${SF_SELECTED}
  SectionSetFlags ${SEC_BIN64W} 0
  StrCpy $SelectedExecutable "psi-plus.exe"
FunctionEnd

; Initialize the installer, validate the runtime, and handle an existing installation.
Function .onInit
  StrCpy $SelectedExecutable "psi-plus.exe"
  ${IfNot} ${RunningX64}
    MessageBox MB_ICONSTOP "$(STR_NON_64BIT_SYSTEM)"
    Abort
  ${EndIf}
  Call SetDefaultBinaryComponent
  Call FindPreviousInstallation
  ${If} $PreviousUninstaller != ""
    Call HandlePreviousInstallation
  ${EndIf}
FunctionEnd

; Locate a previous installation by checking the uninstall registry entries.
Function FindPreviousInstallation
  StrCpy $PreviousUninstaller ""
  StrCpy $PreviousHive ""
  SetRegView 64
  ReadRegStr $PreviousUninstaller HKCU "${UNINSTALL_REGKEY}" "UninstallString"
  ${If} $PreviousUninstaller != ""
    StrCpy $PreviousHive "HKCU"
  ${Else}
    ReadRegStr $PreviousUninstaller HKLM "${UNINSTALL_REGKEY}" "UninstallString"
    ${If} $PreviousUninstaller != ""
      StrCpy $PreviousHive "HKLM"
    ${EndIf}
  ${EndIf}
  SetRegView 32
  ${If} $PreviousUninstaller == ""
    ReadRegStr $PreviousUninstaller HKCU "${UNINSTALL_REGKEY}" "UninstallString"
    ${If} $PreviousUninstaller != ""
      StrCpy $PreviousHive "HKCU"
    ${Else}
      ReadRegStr $PreviousUninstaller HKLM "${UNINSTALL_REGKEY}" "UninstallString"
      ${If} $PreviousUninstaller != ""
        StrCpy $PreviousHive "HKLM"
      ${EndIf}
    ${EndIf}
  ${EndIf}
  ${If} $PreviousUninstaller != ""
    ${If} $PreviousHive == "HKCU"
      ReadRegStr $PreviousVersion HKCU "${UNINSTALL_REGKEY}" "DisplayVersion"
      ReadRegStr $PreviousInstallDir HKCU "${UNINSTALL_REGKEY}" "InstallLocation"
    ${Else}
      ReadRegStr $PreviousVersion HKLM "${UNINSTALL_REGKEY}" "DisplayVersion"
      ReadRegStr $PreviousInstallDir HKLM "${UNINSTALL_REGKEY}" "InstallLocation"
    ${EndIf}
  ${EndIf}
  SetRegView 64
FunctionEnd

; Ask the user to confirm an upgrade, close running Psi+ processes,
; uninstall the old version quietly, and restore the previous selections.
Function HandlePreviousInstallation
  MessageBox MB_ICONINFORMATION|MB_OKCANCEL \
    "$(STR_PREVIOUS_VERSION_FOUND)" \
    IDOK ContinueUpgrade IDCANCEL AbortUpgrade
AbortUpgrade:
  Abort
ContinueUpgrade:
  Call CheckRunningPsi
  ExecWait '$PreviousUninstaller /VERYSILENT /NORESTART /SUPPRESSMSGBOXES /NOCANCEL' $0
  ${If} $0 != 0
    MessageBox MB_ICONSTOP "$(STR_PREVIOUS_REMOVE_FAILED)"
    Abort
  ${EndIf}
  Call RestorePreviousState
FunctionEnd

; Check whether either Psi+ executable is currently active in the system.
Function IsPsiRunning
  StrCpy $0 0
  nsProcess::_FindProcess "psi-plus.exe"
  Pop $1
  ${If} $1 == 0
    StrCpy $0 1
  ${EndIf}
  nsProcess::_FindProcess "psi-plus-webengine.exe"
  Pop $1
  ${If} $1 == 0
    StrCpy $0 1
  ${EndIf}
FunctionEnd

; Prompt the user to exit Psi+ before an upgrade or uninstall can continue.
Function CheckRunningPsi
  Call IsPsiRunning
  ${If} $0 == 0
    Return
  ${EndIf}
  MessageBox MB_ICONQUESTION|MB_YESNO \
    "$(STR_PSI_RUNNING)" \
    IDYES ClosePsi IDNO AbortClosePsi
AbortClosePsi:
  Abort
ClosePsi:
  DetailPrint "Closing Psi+ processes..."
  ExecWait '"$SYSDIR\taskkill.exe" /F /T /IM psi-plus.exe' $1
  ExecWait '"$SYSDIR\taskkill.exe" /F /T /IM psi-plus-webengine.exe' $1
  Sleep 1000
  Call IsPsiRunning
  ${If} $0 != 0
    MessageBox MB_ICONSTOP "$(STR_PSI_STILL_RUNNING)"
    Abort
  ${EndIf}
FunctionEnd

; Restore the previous install directory and selected executable if a prior state exists.
Function RestorePreviousState
  ReadINIStr $0 "${STATE_FILE}" "${STATE_SECTION}" "InstallDir"
  ${If} $0 != ""
    StrCpy $INSTDIR $0
  ${EndIf}
  ReadINIStr $SelectedExecutable "${STATE_FILE}" "${STATE_SECTION}" "Executable"
  ${If} $SelectedExecutable == ""
    StrCpy $SelectedExecutable "psi-plus.exe"
  ${EndIf}
  Call RestoreComponentState
FunctionEnd

!include "restore-components.nsh"

; Restore the saved component flags and ensure exactly one executable is selected.
Function RestoreComponentState
  !insertmacro RESTORE_COMPONENT "bin64" ${SEC_BIN64}
  !insertmacro RESTORE_COMPONENT "bin64w" ${SEC_BIN64W}
  !insertmacro RESTORE_COMPONENT "desktop" ${SEC_DESKTOP}
  !insertmacro RESTORE_COMPONENT "uninstall-icon" ${SEC_UNINSTALL_ICON}
  !insertmacro RESTORE_COMPONENT "xmpp" ${SEC_XMPP}
  !insertmacro RESTORE_ALL_PLUGIN_COMPONENTS
  !insertmacro RESTORE_ALL_DICTIONARY_COMPONENTS

  ${If} ${SectionIsSelected} ${SEC_BIN64W}
    SectionSetFlags ${SEC_BIN64} 0
    StrCpy $SelectedExecutable "psi-plus-webengine.exe"
  ${ElseIf} ${SectionIsSelected} ${SEC_BIN64}
    SectionSetFlags ${SEC_BIN64W} 0
    StrCpy $SelectedExecutable "psi-plus.exe"
  ${Else}
    SectionSetFlags ${SEC_BIN64} ${SF_SELECTED}
    SectionSetFlags ${SEC_BIN64W} 0
    StrCpy $SelectedExecutable "psi-plus.exe"
  ${EndIf}
FunctionEnd

; Display the Visual C++ Redistributable status page before installation continues.
Function VCRedistPageCreate
  nsDialogs::Create 1018
  Pop $0
  ${If} $0 == error
    Abort
  ${EndIf}
  ${NSD_CreateLabel} 0 0 100% 100u "$(STR_VC_CHECKING)"
  Pop $hVCRedistLabel
  Call CheckVCRedist
  ${If} $NeedVCRedist == 1
    ${NSD_SetText} $hVCRedistLabel "$(STR_VC_MISSING)"
  ${Else}
    ${NSD_SetText} $hVCRedistLabel "$(STR_VC_PRESENT)"
  ${EndIf}
  nsDialogs::Show
FunctionEnd

; Placeholder for page cleanup after leaving the VC++ page.
Function VCRedistPageLeave
FunctionEnd

; Check whether the required x64 Visual C++ Redistributable is installed.
Function CheckVCRedist
  StrCpy $NeedVCRedist 1
  SetRegView 64
  ClearErrors
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
  ${IfNot} ${Errors}
    ${If} $0 == 1
      ReadRegDWORD $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Major"
      ReadRegDWORD $2 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Minor"
      ReadRegDWORD $3 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Bld"
      ${If} $1 > 14
        StrCpy $NeedVCRedist 0
      ${ElseIf} $1 == 14
        ${If} $2 > 40
          StrCpy $NeedVCRedist 0
        ${ElseIf} $2 == 40
          StrCpy $NeedVCRedist 0
        ${EndIf}
      ${EndIf}
    ${EndIf}
  ${EndIf}
  SetRegView 32
FunctionEnd

; Display the installer summary before the files are copied.
Function SummaryPageCreate
  nsDialogs::Create 1018
  Pop $0
  ${If} $0 == error
    Abort
  ${EndIf}
  ${NSD_CreateText} 0 0 100% 190u "$(STR_SUMMARY_TEXT)"
  Pop $hSummaryText
  ${If} $NeedVCRedist == 1
    ${NSD_SetText} $hSummaryText "$(STR_SUMMARY_WITH_VC)"
  ${EndIf}
  nsDialogs::Show
FunctionEnd

; Placeholder for page cleanup after leaving the summary page.
Function SummaryPageLeave
FunctionEnd

; Download and install the required Microsoft VC++ runtime package.
Function DownloadAndInstallVCRedist
  DetailPrint "Downloading Microsoft Visual C++ Redistributable..."
  inetc::get /caption "Downloading Microsoft Visual C++ Redistributable" /canceltext "Cancel" "${VC_URL}" "$PLUGINSDIR\vc_redist.x64.exe"
  Pop $0
  ${If} $0 != "OK"
    MessageBox MB_ICONSTOP "$(STR_VC_DOWNLOAD_FAILED)"
    Abort
  ${EndIf}
  ExecWait '"$PLUGINSDIR\vc_redist.x64.exe" /install /quiet /norestart' $0
  ${If} $0 != 0
    MessageBox MB_ICONSTOP "$(STR_VC_INSTALL_FAILED)"
    Abort
  ${EndIf}
FunctionEnd

; Save the installer directory, selected executable, and all section states.
Function SaveInstallerState
  CreateDirectory "$APPDATA\Psi+"
  WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "InstallDir" "$INSTDIR"
  WriteINIStr "${STATE_FILE}" "${STATE_SECTION}" "Executable" "$SelectedExecutable"
  !insertmacro SAVE_COMPONENT "bin64" ${SEC_BIN64}
  !insertmacro SAVE_COMPONENT "bin64w" ${SEC_BIN64W}
  !insertmacro SAVE_COMPONENT "desktop" ${SEC_DESKTOP}
  !insertmacro SAVE_COMPONENT "uninstall-icon" ${SEC_UNINSTALL_ICON}
  !insertmacro SAVE_COMPONENT "xmpp" ${SEC_XMPP}
  !insertmacro SAVE_ALL_PLUGIN_COMPONENTS
  !insertmacro SAVE_ALL_DICTIONARY_COMPONENTS
FunctionEnd

; Keep the standard and WebEngine executable sections mutually exclusive.
Function .onSelChange
  ${If} ${SectionIsSelected} ${SEC_BIN64W}
    SectionSetFlags ${SEC_BIN64} 0
    StrCpy $SelectedExecutable "psi-plus-webengine.exe"
  ${ElseIf} ${SectionIsSelected} ${SEC_BIN64}
    SectionSetFlags ${SEC_BIN64W} 0
    StrCpy $SelectedExecutable "psi-plus.exe"
  ${Else}
    SectionSetFlags ${SEC_BIN64} ${SF_SELECTED}
    SectionSetFlags ${SEC_BIN64W} 0
    StrCpy $SelectedExecutable "psi-plus.exe"
  ${EndIf}
FunctionEnd

; Installer success callback reserved for post-install tasks.
Function .onInstSuccess
FunctionEnd

; Installer failure callback reserved for post-failure tasks.
Function .onInstFailed
FunctionEnd

; Launch the selected Psi+ executable after installation completes.
Function LaunchPsi
  ${If} $SelectedExecutable == "psi-plus-webengine.exe"
    ExecShell open "$INSTDIR\psi-plus-webengine.exe"
  ${Else}
    ExecShell open "$INSTDIR\psi-plus.exe"
  ${EndIf}
FunctionEnd

; Check whether Psi+ is currently running in the uninstall context.
Function un.IsPsiRunning
  StrCpy $0 0
  nsProcess::_FindProcess "psi-plus.exe"
  Pop $1
  ${If} $1 == 0
    StrCpy $0 1
  ${EndIf}
  nsProcess::_FindProcess "psi-plus-webengine.exe"
  Pop $1
  ${If} $1 == 0
    StrCpy $0 1
  ${EndIf}
FunctionEnd

; Ask the user to close Psi+ before uninstalling it.
Function un.CheckRunningPsi
  Call un.IsPsiRunning
  ${If} $0 == 0
    Return
  ${EndIf}
  MessageBox MB_ICONQUESTION|MB_YESNO \
    "$(STR_PSI_RUNNING)" \
    IDYES ClosePsi IDNO AbortClosePsi
AbortClosePsi:
  Abort
ClosePsi:
  DetailPrint "Closing Psi+ processes..."
  ExecWait '"$SYSDIR\taskkill.exe" /F /T /IM psi-plus.exe' $1
  ExecWait '"$SYSDIR\taskkill.exe" /F /T /IM psi-plus-webengine.exe' $1
  Sleep 1000
  Call un.IsPsiRunning
  ${If} $0 != 0
    MessageBox MB_ICONSTOP "$(STR_PSI_STILL_RUNNING)"
    Abort
  ${EndIf}
FunctionEnd

; Initialize the uninstaller and stop running Psi+ processes before removing it.
Function un.onInit
  Call un.CheckRunningPsi
FunctionEnd

Section "Uninstall"
  Delete "$DESKTOP\Psi+ (x64).lnk"
  Delete "$SMPROGRAMS\Psi+ (x64)\Uninstall Psi+.lnk"
  RMDir "$SMPROGRAMS\Psi+ (x64)"
  DeleteRegKey HKCR "xmpp"
  DeleteRegKey HKCU "${UNINSTALL_REGKEY}"
  DeleteRegKey HKLM "${UNINSTALL_REGKEY}"
  RMDir /r "$INSTDIR"
SectionEnd
