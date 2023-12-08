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
OutFile "output\RBTray_4.6.exe"

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
BrandingText "https://www.github.com/SMSourov/rbtray"

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
!include "installSection.nsi"

# Show the finish page
!insertmacro MUI_PAGE_FINISH

# Include the functions for the radio button behavior
!include "functions.nsi"

# Show the uninstallation welcome page
!insertmacro MUI_UNPAGE_WELCOME

# Show the uninstallation page
!insertmacro MUI_UNPAGE_INSTFILES

# show the uninstallation finish page
!insertmacro MUI_UNPAGE_FINISH

# Create a section for the uninstallation
!include "uninstallSection.nsi"

# Set the language of the installer
!insertmacro MUI_LANGUAGE "English"
