# Define the app name and app version
!define APP_NAME "RBTray"
!define APP_VERSION "4.6.0.0"
Name "${APP_NAME}"

# Set the product version for the installer
VIProductVersion "${APP_VERSION}"

# Set the version information for the application
VIAddVersionKey "ProductName" "${APP_NAME}"
VIAddVersionKey "ProductVersion" "${APP_VERSION}"
VIAddVersionKey "FileVersion" "${APP_VERSION}"
VIAddVersionKey "FileDescription" "${APP_NAME} Installer"
VIAddVersionKey "LegalCopyright" "Owner"

# Define the location and name of the installer
OutFile "output\RBTray v4.6.0.0 x64 installer.exe"

# Define the installation directory
InstallDir "$PROGRAMFILES64\${APP_NAME}"

# Include headers
!include "x64.nsh"
!include "Sections.nsh"


# Request for admin privilege
RequestExecutionLevel admin

# Adjust the DPI of the installer
ManifestDPIAware true

# Include Modern UI
!include "MUI2.nsh"

# Define the install and uninstall icons
!define MUI_ICON "assets\RBTray-new.ico"
!define MUI_UNICON "assets\RBTray-new.ico"

# Define the wizard image for the installation
!define MUI_WELCOMEFINISHPAGE_BITMAP "assets\orange-wizard-install.bmp"

# Define the wizard image for the installation
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "assets\orange-wizard-uninstall.bmp"

# Change the "Nullsoft Install System" text
BrandingText "RBTray v4.6.0.0 x64"

# Define warning text for closing the installation
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_TEXT "Quit the installation process?"

# Define warning text for closing the installation
!define MUI_UNABORTWARNING
!define MUI_UNABORTWARNING_TEXT "Quit the uninstallation process?"

# Start defining header images
!define MUI_HEADERIMAGE

# Define the header image for installation
!define MUI_HEADERIMAGE_BITMAP "assets\orange-header-install.bmp"

# Define the header image for uninstallation
!define MUI_HEADERIMAGE_UNBITMAP "assets\orange-header-uninstall.bmp"

# Show the welcome page
!insertmacro MUI_PAGE_WELCOME

# Show "OK!" instead of "I Agree" button in the EULA page
!define MUI_LICENSEPAGE_BUTTON "OK!"

# Define the options for the for the user in the EULA page
!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_LICENSEPAGE_CHECKBOX_TEXT "I agree"

# Show the license page
!insertmacro MUI_PAGE_LICENSE "docs\EULA.rtf"

# Define the texts of the components page
!define MUI_COMPONENTSPAGE_TEXT_TOP "Select auto start method"
!define MUI_COMPONENTSPAGE_TEXT_DESCRIPTION_TITLE "Information"
!define MUI_COMPONENTSPAGE_TEXT_COMPLIST "Note: In no hook method, right click won't work. Only Ctrl-Alt-Down will work."
!define MUI_COMPONENTSPAGE_SMALLDESC

# Show the components page
!insertmacro MUI_PAGE_COMPONENTS

# Show the installation directory page
!insertmacro MUI_PAGE_DIRECTORY

# Show the installation page
!insertmacro MUI_PAGE_INSTFILES

# Show "GREAT!" instead of "Finish" button in the Finish page
!define MUI_FINISHPAGE_BUTTON "GREAT!"

# Create a section for the installation
Section "Main program" sec01
    # Make this section mandatory
    SectionIn RO
    # Set the output path to the installation directory
    SetOutPath $INSTDIR
    
    # Install the files from the folder
    File "app\x64\RBHook.dll"
    File "app\x64\RBTray.exe"
    File "docs\README - RBTray.pdf"
    
    # Create a shortcut for the application in the start menu
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\RBTray - Stop.lnk" "$INSTDIR\RBTray.exe" "--exit"
    
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


# Show the finish page
!insertmacro MUI_PAGE_FINISH

# Functions for making the check box behave like radio buttons
Function .onInit
    StrCpy $1 ${sec02}
FunctionEnd

Function .onSelChange
    !insertmacro StartRadioButtons $1
        !insertmacro RadioButton ${sec02}
        !insertmacro RadioButton ${sec03}
        !insertmacro RadioButton ${sec04}
    !insertmacro EndRadioButtons
FunctionEnd


# Show the uninstallation welcome page
!insertmacro MUI_UNPAGE_WELCOME

# Show the uninstallation page
!insertmacro MUI_UNPAGE_INSTFILES

# show the uninstallation finish page
!insertmacro MUI_UNPAGE_FINISH

# Create a section for the uninstallation
Section "Uninstall"

    # This command will make a flash of command prompt to the user
    ; ExecWait "TaskKill /IM RBTray.exe /F"

    # This command will not flash anything to the user
    nsExec::ExecToStack /TIMEOUT=2000 "TaskKill /IM RBTray.exe /F"
    # In both commands, unfortunately the child process will be killed


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


# Set the language of the installer
!insertmacro MUI_LANGUAGE "English"
