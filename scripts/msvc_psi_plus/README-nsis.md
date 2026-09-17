# NSIS installer for Psi+

`psi-plus-setup-64.nsi` is the NSIS counterpart of the Inno Setup script. It keeps build artifacts wildcard-based: plugins, dictionaries, runtime libraries, translations, WebEngine files, sounds and resources are added with `File`, `File /r` and `File /nonfatal` masks rather than being enumerated as individual files.

## Build prerequisites

- NSIS 3.x Unicode compiler.
- `InetC` NSIS plug-in for HTTPS download.
- `nsProcess` NSIS plug-in for detecting running Psi+ processes.
- The build directories used by the original script (`C:\build\Installer`, `C:\build\resources`, `C:\build\psi`, `common`, and `plugins`) or equivalent paths passed as defines.

Example:

```bat
makensis /DWORK_DIR=C:\build\Installer /DRES_DIR=C:\build\resources /DPSI_SRC_DIR=C:\build\psi scripts\msvc_psi_plus\psi-plus-setup-64.nsi
```

## Upgrade flow

The installer searches the old Inno Setup uninstall key, asks for confirmation, checks for running Psi+ processes, saves the old install directory and Inno component/task strings to `%TEMP%\psi-plus-upgrade.ini`, runs the old uninstaller, and restores the selections before showing the component page. The component and task pages are not skipped, so the user can change the restored defaults.

The complete selected plugin/dictionary mapping can be extended in `RestoreComponentState`; the current file includes representative core plugins and the common dictionaries. The source masks themselves remain wildcard-based.

## VC++ Redistributable

The VC++ runtime is checked only after the user has completed the component selection. If the x64 runtime is missing or older than `14.42.34433.0`, the summary page explains that the official Microsoft package will be downloaded from `https://aka.ms/vs/17/release/vc_redist.x64.exe`; the hidden installation section then downloads it through `InetC` and installs it silently.

## Registry cleanup

The uninstaller removes only installer-owned data: the uninstall registration, the optional XMPP protocol registration, and the installer metadata key. It deliberately does not remove application settings, account data, or QtKeychain entries because those are created by the application and their exact ownership has not been established.

> Review the `RequestExecutionLevel` and registry hive choices before shipping. This version uses a per-user installation (`$LOCALAPPDATA` and `HKCU`) so it can work without elevation. If the product must install into Program Files or register XMPP system-wide, use an elevated install and write the corresponding metadata consistently to `HKLM`.
