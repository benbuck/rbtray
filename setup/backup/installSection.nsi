Section "Main program" sec01
  SectionIn RO
  # Set the output path to the installation directory
  SetOutPath $INSTDIR

  # Install the files from the folder
  File "app\x64\RBHook.dll"
  File "app\x64\RBTray.exe"
  File "docs\README - RBTray.pdf"

  # Create a shortcut for the application in the start menu
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  # CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\RBTray.exe"
  # CreateShortCut "$SMPROGRAMS\${APP_NAME}\RBTray - No Hook.lnk" "$INSTDIR\RBTray.exe" "--no-hook"
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\RBTray - Stop.lnk" "$INSTDIR\RBTray.exe" "--exit"

  # Create a shortcut for the application with no hook argument in the installation directory
  ; CreateShortCut "$INSTDIR\No Hook.lnk" "$INSTDIR\RBTray.exe" "--no-hook"

  # Create a shortcut for the application with exit argument in the installation directory
  CreateShortCut "$INSTDIR\Stop.lnk" "$INSTDIR\RBTray.exe" "--exit"

  # Write the uninstaller to the installation directory
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  # Write the uninstall information to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME} ${APP_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""

SectionEnd

# Be default, auto start using the hook method.
Section "Hook method" sec02
  # Create a start menu shortcut for hook method
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\RBTray.exe"

  # Create an auto run task in hook method
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME}" "$\"$INSTDIR\RBTray.exe$\""
SectionEnd

# Next options should be optional. Use "/o" to make them optional
Section /o "No hook method" sec03
  # Create a shortcut in the install directory for no hook method
  CreateShortCut "$INSTDIR\No Hook.lnk" "$INSTDIR\RBTray.exe" "--no-hook"
  # Create a start menu shortcut for no hook method
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\RBTray - No Hook.lnk" "$INSTDIR\RBTray.exe" "--no-hook"

  # Create an auto run task in no hook method 
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME} no hook" "$\"$INSTDIR\RBTray.exe$\" --no-hook"
SectionEnd

Section /o "No autostart" sec04
  # Create a start menu shortcut for hook method
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\RBTray.exe"

  # Create a shortcut in the install directory for no hook method
  CreateShortCut "$INSTDIR\No Hook.lnk" "$INSTDIR\RBTray.exe" "--no-hook"
  
  # Create a start menu shortcut for no hook method
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\RBTray - No Hook.lnk" "$INSTDIR\RBTray.exe" "--no-hook"
SectionEnd

# Define the description for each section
LangString DESC_sec01 1033 "This contains the main executable and dll file."
LangString DESC_sec02 1033 "The program will start in hook method."
LangString DESC_sec03 1033 "The program will start in no hook method. Very usefull if you play games that contains anti-cheat software."
LangString DESC_sec04 1033 "The program will not auto start."

# Ignore the problem in VS Code
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${sec01} $(DESC_sec01)
  !insertmacro MUI_DESCRIPTION_TEXT ${sec02} $(DESC_sec02)
  !insertmacro MUI_DESCRIPTION_TEXT ${sec03} $(DESC_sec03)
  !insertmacro MUI_DESCRIPTION_TEXT ${sec04} $(DESC_sec04)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
