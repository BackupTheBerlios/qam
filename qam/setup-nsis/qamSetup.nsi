;------------------------
; general

Name "qam"
outFile "qamSetup.exe"

;Default Installation Directory
InstallDir "$PROGRAMFILES\nichtcool\qam"

;Override InstallDir-setting with the registry-key value if present
InstallDirRegKey HKCU "Software\nichtcool\qam\Installer" InstallLocation

;Vista redirects $SMPROGRAMS to all users without this
RequestExecutionLevel admin

;-----------------------------
; variables

Var MUI_TEMP
Var INST_TMP
Var STARTMENU_FOLDER

;-----------------------
; setup MUI

!include MUI.nsh

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "welcomefinish.bmp"

; welcome page
!insertmacro MUI_PAGE_WELCOME

; license page
!insertmacro MUI_PAGE_LICENSE "license.txt"

; install-dir page
!insertmacro MUI_PAGE_DIRECTORY

; start-menu page
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Modern UI Test" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "nichtcool\qam"
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

; install page
!insertmacro MUI_PAGE_INSTFILES

; finish page
!insertmacro MUI_PAGE_FINISH

; uninstaller
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "German"

;----------------------------------------------------
; functions

Function .onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "qam Setup running...") i .r1 ?e'
	Pop $R0
 
	StrCmp $R0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
		Abort
FunctionEnd

;-----------------------------------------------------
; sections

Section "Extract qam"
	SetOutPath $INSTDIR
	File "qam.exe"
	
	File "license.txt"
	File "gpl2.txt"
	
	;MS VC runtimes
	File "Microsoft.VC80.CRT.manifest"
	File "msvcm80.dll"
	File "msvcp80.dll"
	File "msvcr80.dll"
	
	;Start Menu Entries
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\qam.lnk" "$INSTDIR\qam.exe"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\qam deinstallieren.lnk" "$INSTDIR\uninstallQam.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
	
	;Store installation path
	WriteRegStr HKCU "Software\nichtcool\qam\Installer" "InstallLocation" $INSTDIR
	WriteRegStr HKCU "Software\nichtcool\qam\Installer" "StartMenuFolder" $STARTMENU_FOLDER
	
	;Create uninstaller
	WriteUninstaller $INSTDIR\uninstallQam.exe
SectionEnd

Section "Uninstall"
	;Remove application files
	Delete "$INSTDIR\qam.exe"
	
	Delete "$INSTDIR\license.txt"
	Delete "$INSTDIR\gpl2.txt"
	
	;Remove VCRT redist runtimes
	Delete "$INSTDIR\Microsoft.VC80.CRT.manifest"
	Delete "$INSTDIR\msvcm80.dll"
	Delete "$INSTDIR\msvcp80.dll"
	Delete "$INSTDIR\msvcr80.dll"
	
	; Remove Uninstaller
	Delete "$INSTDIR\uninstallQam.exe"
	
	;Delete installation directory if empty and upper empty directories
	StrCpy $INST_TMP $INSTDIR
	
	programFilesDeleteLoop:
		ClearErrors
		RMDir $INST_TMP
		GetFullPathName $INST_TMP "$INST_TMP\.."
		
		IfErrors programFilesDeleteLoopDone
		
		StrCmp $INST_TMP $PROGRAMFILES programFilesDeleteLoopDone programFilesDeleteLoop
	programFilesDeleteLoopDone:
	
	; Start Menu Entries
	!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
	Delete "$MUI_TEMP\qam.lnk"
	Delete "$MUI_TEMP\qam deinstallieren.lnk"
	
	;Delete empty start menu parent diretories
	StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
	startMenuDeleteLoop:
		ClearErrors
		RMDir $MUI_TEMP
		GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

		IfErrors startMenuDeleteLoopDone

		StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
	startMenuDeleteLoopDone:
	
	;Delete Registry Keys
	DeleteRegKey HKCU "Software\nichtcool\qam"
	DeleteRegKey /ifempty HKCU "Software\nichtcool"
SectionEnd
