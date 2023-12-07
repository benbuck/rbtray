Section "Uninstall"

  # This command will make a flash of command prompt to the user
  ; ExecWait "TaskKill /IM RBTray.exe /F"

  # This command will not flash anything to the user
  nsExec::ExecToStack /TIMEOUT=2000 "TaskKill /IM RBTray.exe /F"



  # Remove the files from the installation directory
  Delete "$INSTDIR\RBHook.dll"
  Delete "$INSTDIR\RBTray.exe"
  Delete "$INSTDIR\README - RBTray.pdf"
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\No Hook.lnk"
  Delete "$INSTDIR\Stop.lnk"


  # Remove the installation directory
  RMDir "$INSTDIR"

  # Remove the shortcut from the start menu
  Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
  Delete "$SMPROGRAMS\${APP_NAME}\RBTray - No Hook.lnk"
  Delete "$SMPROGRAMS\${APP_NAME}\RBTray - Stop.lnk"
  RMDir "$SMPROGRAMS\${APP_NAME}"

  # Remove the uninstall information from the registry
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME} no hook"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME}"

  ; SetRebootFlag true

SectionEnd
