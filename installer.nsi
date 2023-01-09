; includes
!include nsDialogs.nsh ; para crear controles.
!include LogicLib.nsh  ; permite hacer if...then...endif
!include FileFunc.nsh ; permite utilizar macro DirState

; Compresion
SetCompressor /SOLID lzma

; Remover accesos directos en la desinstalacion Windows Vista/7
RequestExecutionLevel admin

; Macros
; See http://nsis.sourceforge.net/Check_if_a_file_exists_at_compile_time for documentation
!macro !defineifexist _VAR_NAME _FILE_NAME
	!tempfile _TEMPFILE
	!ifdef NSIS_WIN32_MAKENSIS
		; Windows - cmd.exe
		!system 'if exist "${_FILE_NAME}" echo !ifdef ${_VAR_NAME} > "${_TEMPFILE}"'
		!system 'if exist "${_FILE_NAME}" echo !undef ${_VAR_NAME} >> "${_TEMPFILE}"'
		!system 'if exist "${_FILE_NAME}" echo !endif >> "${_TEMPFILE}"'
		!system 'if exist "${_FILE_NAME}" echo !define ${_VAR_NAME} "${_FILE_NAME}" >> "${_TEMPFILE}"'
	!else
		; Posix - sh
		!system 'if [ -e "${_FILE_NAME}" ]; then echo "!ifdef ${_VAR_NAME}" > "${_TEMPFILE}"; fi'
		!system 'if [ -e "${_FILE_NAME}" ]; then echo "!undef ${_VAR_NAME}" >> "${_TEMPFILE}"; fi'
		!system 'if [ -e "${_FILE_NAME}" ]; then echo "!endif" >> "${_TEMPFILE}"; fi'
		!system 'if [ -e "${_FILE_NAME}" ]; then echo "!define ${_VAR_NAME}" "${_FILE_NAME}" >> "${_TEMPFILE}"; fi'
	!endif
	!include '${_TEMPFILE}'
	!delfile '${_TEMPFILE}'
	!undef _TEMPFILE
!macroend
!define !defineifexist "!insertmacro !defineifexist"

!macro ReplaceBetween This AndThis With In
Push "${This}"
Push "${AndThis}"
Push "${With}"
Push "${In}"
 Call ReplaceBetween
!macroend
!define ReplaceBetween "!insertmacro ReplaceBetween"
 
Function ReplaceBetween
 Exch $R0 ; file
 Exch
 Exch $R1 ; replace with
 Exch 2
 Exch $R2 ; before this (marker 2)
 Exch 2
 Exch 3
 Exch $R3 ; after this  (marker 1)
 Exch 3
 Push $R4 ; marker 1 len
 Push $R5 ; marker pos
 Push $R6 ; file handle
 Push $R7 ; temp file handle
 Push $R8 ; temp file name
 Push $R9 ; current line string
 Push $0 ; current chop
 Push $1 ; marker 1 + text
 Push $2 ; marker 2 + text
 Push $3 ; marker 2 len
 
 GetTempFileName $R8
 FileOpen $R7 $R8 w
 FileOpen $R6 $R0 r
 
 StrLen $3 $R3
 StrLen $R4 $R2
 
 Read1:
  ClearErrors
  FileRead $R6 $R9
  IfErrors Done
  StrCpy $R5 -1
 
 FindMarker1:
  IntOp $R5 $R5 + 1
  StrCpy $0 $R9 $3 $R5
  StrCmp $0 "" Write
  StrCmp $0 $R3 0 FindMarker1
   IntOp $R5 $R5 + $3
   StrCpy $1 $R9 $R5
 
  StrCpy $R9 $R9 "" $R5
  StrCpy $R5 0
  Goto FindMarker2
 
 Read2:
  ClearErrors
  FileRead $R6 $R9
  IfErrors Done
  StrCpy $R5 0
 
 FindMarker2:
  IntOp $R5 $R5 - 1
  StrCpy $0 $R9 $R4 $R5
  StrCmp $0 "" Read2
  StrCmp $0 $R2 0 FindMarker2
   StrCpy $2 $R9 "" $R5
 
   FileWrite $R7 $1$R1$2
   Goto Read1
 
 Write:
  FileWrite $R7 $R9
  Goto Read1
 
 Done:
  FileClose $R6
  FileClose $R7
 
  SetDetailsPrint none
  Delete $R0
  Rename $R8 $R0
  SetDetailsPrint both
 
 Pop $3
 Pop $2
 Pop $1
 Pop $0
 Pop $R9
 Pop $R8
 Pop $R7
 Pop $R6
 Pop $R5
 Pop $R4
 Pop $R3
 Pop $R2
 Pop $R1
 Pop $R0
FunctionEnd

Function StrRep
  Exch $R4 ; $R4 = Replacement String
  Exch
  Exch $R3 ; $R3 = String to replace (needle)
  Exch 2
  Exch $R1 ; $R1 = String to do replacement in (haystack)
  Push $R2 ; Replaced haystack
  Push $R5 ; Len (needle)
  Push $R6 ; len (haystack)
  Push $R7 ; Scratch reg
  StrCpy $R2 ""
  StrLen $R5 $R3
  StrLen $R6 $R1
loop:
  StrCpy $R7 $R1 $R5
  StrCmp $R7 $R3 found
  StrCpy $R7 $R1 1 ; - optimization can be removed if U know len needle=1
  StrCpy $R2 "$R2$R7"
  StrCpy $R1 $R1 $R6 1
  StrCmp $R1 "" done loop
found:
  StrCpy $R2 "$R2$R4"
  StrCpy $R1 $R1 $R6 $R5
  StrCmp $R1 "" done loop
done:
  StrCpy $R3 $R2
  Pop $R7
  Pop $R6
  Pop $R5
  Pop $R2
  Pop $R1
  Pop $R4
  Exch $R3
FunctionEnd

; Push $filenamestring (e.g. 'c:\this\and\that\filename.htm')
; Push "\"
; Call StrSlash
; Pop $R0
; ;Now $R0 contains 'c:/this/and/that/filename.htm'
Function StrSlash
  Exch $R3 ; $R3 = needle ("\" or "/")
  Exch
  Exch $R1 ; $R1 = String to replacement in (haystack)
  Push $R2 ; Replaced haystack
  Push $R4 ; $R4 = not $R3 ("/" or "\")
  Push $R6
  Push $R7 ; Scratch reg
  StrCpy $R2 ""
  StrLen $R6 $R1
  StrCpy $R4 "\"
  StrCmp $R3 "/" loop
  StrCpy $R4 "/"  
loop:
  StrCpy $R7 $R1 1
  StrCpy $R1 $R1 $R6 1
  StrCmp $R7 $R3 found
  StrCpy $R2 "$R2$R7"
  StrCmp $R1 "" done loop
found:
  StrCpy $R2 "$R2$R4"
  StrCmp $R1 "" done loop
done:
  StrCpy $R3 $R2
  Pop $R7
  Pop $R6
  Pop $R4
  Pop $R2
  Pop $R1
  Exch $R3
FunctionEnd

!macro _StrReplaceConstructor ORIGINAL_STRING TO_REPLACE REPLACE_BY
  Push "${ORIGINAL_STRING}"
  Push "${TO_REPLACE}"
  Push "${REPLACE_BY}"
  Call StrRep
  Pop $0
!macroend
 
!define StrReplace '!insertmacro "_StrReplaceConstructor"'

!include 'FileFunc.nsh'
!insertmacro Locate
 
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; MoveFile and MoveFolder macros
;
; Author:  theblazingangel@aol.com (for the AutoPatcher project - www.autopatcher.com)
; Created: June 2007  
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 
;==================
; MoveFile macro
;==================
 
    !macro MoveFile sourceFile destinationFile
 
	!define MOVEFILE_JUMP ${__LINE__}
 
	; Check source actually exists
 
	    IfFileExists "${sourceFile}" +3 0
	    SetErrors
	    goto done_${MOVEFILE_JUMP}
 
	; Add message to details-view/install-log
 
	    DetailPrint "Moving/renaming file: ${sourceFile} to ${destinationFile}"
 
	; If destination does not already exists simply move file
 
	    IfFileExists "${destinationFile}" +3 0
	    rename "${sourceFile}" "${destinationFile}"
	    goto done_${MOVEFILE_JUMP}
 
	; If overwriting without 'ifnewer' check
 
	    ${If} $switch_overwrite == 1
		delete "${destinationFile}"
		rename "${sourceFile}" "${destinationFile}"
		delete "${sourceFile}"
		goto done_${MOVEFILE_JUMP}
	    ${EndIf}
 
	; If destination already exists
 
	    Push $R0
	    Push $R1
	    Push $R2
	    push $R3
 
	    GetFileTime "${sourceFile}" $R0 $R1
	    GetFileTime "${destinationFile}" $R2 $R3
 
	    IntCmp $R0 $R2 0 older_${MOVEFILE_JUMP} newer_${MOVEFILE_JUMP}
	    IntCmp $R1 $R3 older_${MOVEFILE_JUMP} older_${MOVEFILE_JUMP} newer_${MOVEFILE_JUMP}
 
	    older_${MOVEFILE_JUMP}:
	    delete "${sourceFile}"
	    goto time_check_done_${MOVEFILE_JUMP}
 
	    newer_${MOVEFILE_JUMP}:
	    delete "${destinationFile}"
	    rename "${sourceFile}" "${destinationFile}"
	    delete "${sourceFile}" ;incase above failed!
 
	    time_check_done_${MOVEFILE_JUMP}:
 
	    Pop $R3
	    Pop $R2
	    Pop $R1
	    Pop $R0
 
	done_${MOVEFILE_JUMP}:
 
	!undef MOVEFILE_JUMP
 
    !macroend
 
;==================
; MoveFolder macro
;==================
 
    !macro MoveFolder source destination mask
 
	!define MOVEFOLDER_JUMP ${__LINE__}
 
	Push $R0
	Push $R1
 
	; Move path parameters into registers so they can be altered if necessary
 
	    StrCpy $R0 "${source}"
	    StrCpy $R1 "${destination}"
 
	; Sort out paths - remove final backslash if supplied
 
	    Push $0
 
	    ; Source
	    StrCpy $0 "$R0" 1 -1
	    StrCmp $0 '\' 0 +2
	    StrCpy $R0 "$R0" -1
 
	    ; Destination
	    StrCpy $0 "$R1" 1 -1
	    StrCmp $0 '\' 0 +2
	    StrCpy $R1 "$R1" -1
 
	    Pop $0
 
	; Create destination dir
 
	    CreateDirectory "$R1\"
 
	; Add message to details-view/install-log
 
	    DetailPrint "Moving files: $R0\${mask} to $R1\"
 
	; Push registers used by ${Locate} onto stack
 
	    Push $R6
	    Push $R7
	    Push $R8
	    Push $R9
 
	; Duplicate dir structure (to preserve empty folders and such)
 
	    ${Locate} "$R0" "/L=D" ".MoveFolder_Locate_createDir"
 
	; Locate files and move (via callback function)
 
	    ${Locate} "$R0" "/L=F /M=${mask} /S= /G=1" ".MoveFolder_Locate_moveFile"
 
	; Delete subfolders left over after move
 
	    Push $R2
	    deldir_loop_${MOVEFOLDER_JUMP}:
	    StrCpy $R2 0
	    ${Locate} "$R0" "/L=DE" ".MoveFolder_Locate_deleteDir"
	    StrCmp $R2 0 0 deldir_loop_${MOVEFOLDER_JUMP}
	    Pop $R2
 
	; Delete empty subfolders moved - say the user just wanted to move *.apm files, they now also have a load of empty dir's from dir structure duplication!
 
	    Push $R2
	    delnewdir_loop_${MOVEFOLDER_JUMP}:
	    StrCpy $R2 0
	    ${Locate} "$R1" "/L=DE" ".MoveFolder_Locate_deleteDir"
	    StrCmp $R2 0 0 delnewdir_loop_${MOVEFOLDER_JUMP}
	    Pop $R2
 
	; Pop registers used by ${Locate} off the stack again
 
	    Pop $R9
	    Pop $R8
	    Pop $R7
	    Pop $R6
 
	; Delete source folder if empty
 
	    rmdir "$R0"
 
	Pop $R1
	Pop $R0
 
	!undef MOVEFOLDER_JUMP
 
    !macroend
 
;==================
; MoveFolder macro's ${Locate} callback functions
;==================
 
	Function .MoveFolder_Locate_createDir
 
	    ${If} $R6 == ""
		Push $R2
		StrLen $R2 "$R0"
		StrCpy $R2 $R9 '' $R2
		CreateDirectory "$R1$R2"
		Pop $R2
	    ${EndIf}
 
	    Push $R1
 
	FunctionEnd
 
	Function .MoveFolder_Locate_moveFile
 
	    Push $R2
 
	    ; Get path to file
 
		StrLen $R2 "$R0"
		StrCpy $R2 $R9 '' $R2
		StrCpy $R2 "$R1$R2"
 
	    ; If destination does not already exists simply move file
 
		IfFileExists "$R2" +3 0
		rename "$R9" "$R2"
		goto done
 
	    ; If overwriting without 'ifnewer' check
 
		${If} $switch_overwrite == 1
		    delete "$R2"
		    rename "$R9" "$R2"
		    delete "$R9"
		    goto done
		${EndIf}
 
	    ; If destination already exists
 
		Push $0
		Push $1
		Push $2
		push $3
 
		GetFileTime "$R9" $0 $1
		GetFileTime "$R2" $2 $3
 
		IntCmp $0 $2 0 older newer
		IntCmp $1 $3 older older newer
 
		older:
		delete "$R9"
		goto time_check_done
 
		newer:
		delete "$R2"
		rename "$R9" "$R2"
		delete "$R9" ;incase above failed!
 
		time_check_done:
 
		Pop $3
		Pop $2
		Pop $1
		Pop $0
 
	    done:
 
	    Pop $R2
 
	    Push $R1
 
	FunctionEnd
 
	Function .MoveFolder_Locate_deleteDir
 
	    ${If} $R6 == ""
		RMDir $R9
		IntOp $R2 $R2 + 1
	    ${EndIf}
 
	    Push $R1
 
	FunctionEnd


!define Move '!insertmacro "MoveFile" '
!define MoveDir '!insertmacro "MoveFolder" '

; defines
!ifndef DLLS_DIR
!define DLLS_DIR "C:\msys\1.0\local\bin\"
!endif
!ifndef WX_DLLS_DIR
!define WX_DLLS_DIR "C:\msys\1.0\local\lib\"
!endif

!ifndef BUILD_ENVIRONMENT
!define BUILD_ENVIRONMENT 19999
!endif

; version de entorno no definido
${!defineifexist} BUILD_ENVIRONMENT 19999

; Definicion de nombres de bibliotecas transversales a versiones de entorno
${!defineifexist} MINIZIP_DLL_NAME ${DLLS_DIR}libminizip.dll
${!defineifexist} URI_DLL_NAME ${DLLS_DIR}liburiparser.dll
${!defineifexist} SURI_DLL "lib\suri\libsuri.dll"
${!defineifexist} LIBTIFF_DLL_NAME ${DLLS_DIR}libtiff-3.dll
${!defineifexist} LIBPROJ_DLL_NAME ${DLLS_DIR}libproj-0.dll
${!defineifexist} LIBGEOS_DLL_NAME ${DLLS_DIR}libgeos_c-1.dll
${!defineifexist} LIBCURL_DLL_NAME ${DLLS_DIR}libcurl-4.dll
${!defineifexist} LIBEXPAT_DLL_NAME ${DLLS_DIR}libexpat-1.dll
${!defineifexist} LIBHDF5_DLL_NAME ${DLLS_DIR}libhdf5.dll
${!defineifexist} LIBJPEG_DLL_NAME ${DLLS_DIR}libjpeg-8.dll
${!defineifexist} LIBTIFFXX_DLL_NAME ${DLLS_DIR}libtiffxx-3.dll
${!defineifexist} LIBZ_DLL_NAME ${DLLS_DIR}libz.dll
${!defineifexist} LIBMUPARSER_DLL_NAME ${WX_DLLS_DIR}muparser.dll
${!defineifexist} GDAL_DLL_NAME ${DLLS_DIR}libgdal-1.dll
${!defineifexist} KML_DLL_NAME ${DLLS_DIR}libkmlbase.dll
${!defineifexist} KMLDOM_DLL_NAME ${DLLS_DIR}libkmldom.dll
${!defineifexist} KMLCONV_DLL_NAME ${DLLS_DIR}libkmlconvenience.dll
${!defineifexist} KMLENG_DLL_NAME ${DLLS_DIR}libkmlengine.dll
${!defineifexist} KMLREG_DLL_NAME ${DLLS_DIR}libkmlregionator.dll

!if ${BUILD_ENVIRONMENT} < 20000
; Define las rutas de las bibliotecas (prefiere las del entorno 1.2.x)
   ${!defineifexist} KML_DLL_NAME ${DLLS_DIR}libkml.dll
   ${!defineifexist} GEOS_DLL_NAME ${DLLS_DIR}libgeos-3-2-2.dll
   ${!defineifexist} LIBPNG3_DLL_NAME ${DLLS_DIR}libpng-3.dll
   ${!defineifexist} LIBPNG_DLL_NAME ${DLLS_DIR}libpng12-0.dll
   ${!defineifexist} LIBGEOTIFF_DLL_NAME ${DLLS_DIR}libgeotiff-2.dll
   ${!defineifexist} LIBGCC_DLL_NAME ${DLLS_DIR}libgcc_s_dw2-1.dll
!else if ${BUILD_ENVIRONMENT} < 30000
   ${!defineifexist} LIBSTDCPP_DLL_NAME ${DLLS_DIR}libstdc++-6.dll
   ${!defineifexist} LIBGCC_DLL_NAME ${DLLS_DIR}libgcc_s_sjlj-1.dll
   ${!defineifexist} LIBGEOTIFF_DLL_NAME ${DLLS_DIR}libgeotiff.dll
   ${!defineifexist} LIBPNG_DLL_NAME ${DLLS_DIR}libpng16-16.dll
   ${!defineifexist} GEOS_DLL_NAME ${DLLS_DIR}libgeos-3-3-8.dll
   ${!defineifexist} LIBWINPTHREAD_DLL_NAME ${DLLS_DIR}libwinpthread-1.dll
   ${!defineifexist} LIBSQLITE3_DLL_NAME ${DLLS_DIR}libsqlite3-0.dll
   ${!defineifexist} LIBOPENJP2_DLL_NAME ${DLLS_DIR}libopenjp2.dll
!endif

!define APP_SHORT_NAME "2Mp"
!define APP_NAME "Programa 2Mp"
; Nombre del ejecutable de la compilacion
!ifndef EXEFILE
!define EXEFILE "Programa2Mp.exe"
!endif

!define VENDOR Programa2Mp
!define VERSION "3.0.0"
!define INSTALLER_FILE "Software ${VENDOR}-v${VERSION}.exe"
!define INSTALLER_ICON ".\lib\resources\bitmaps\icon-Installer.ico"
!define UNINSTALLER_ICON ".\lib\resources\bitmaps\icon-UnInstaller.ico"
!define PAT_ICON "icon-PATFile-32.ico"
!define PAT_ICON_PATH ".\lib\resources\bitmaps\${PAT_ICON}"
!define SRC_DIR ".\src\"
!define UNINSTALLER_NAME "Uninstall.exe"
!define USR_DIR "$APPDATA\${APP_NAME}"
!define PAT_DIR "${USR_DIR}\pats"
!define SUR_REG_KEY_ROOT "Software\SUR Software"
!define REG_KEY_2MP "${SUR_REG_KEY_ROOT}\${APP_SHORT_NAME}"
!define FILE_EXT ".pat"

; Traducciones
;LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Spanish.nlf"

;LangString application_name ${LANG_ENGLISH} ${APP_NAME}
LangString application_name ${LANG_SPANISH} "${APP_NAME}"

;LangString application_dir ${LANG_ENGLISH} ${APP_SHORT_NAME}
LangString application_dir ${LANG_SPANISH} ${APP_SHORT_NAME}

;LangString message ${LANG_ENGLISH} "This will install $(application_name) software. Continue?"
LangString message ${LANG_SPANISH} "Este programa instalará el $(application_name). ¿Desea continuar?"

;LangString message ${LANG_ENGLISH} "$(application_name) is already installed. $\n$\nClick `OK` to remove the current version or `Cancel` to cancel this upgrade."
LangString uninstall_message ${LANG_SPANISH} "Existe una version de $(application_name) instalada. $\n$\n Presione 'Aceptar' para desinstalar la version actual o 'Cancelar' para cancelar la instalación."

;LangString message_delete_usr_data ${LANG_ENGLISH} "Do you want to delete PATs's directory?"
LangString message_delete_usr_data ${LANG_SPANISH} "El $(application_name) ha detectado la existencia del directorio de trabajo de los usuarios. ¿Desea eliminar directorio de PATs?"

;LangString message_delete_pats_dir ${LANG_ENGLISH} "Uninstalling"
LangString message_delete_pats_dir ${LANG_SPANISH} "Desinstalando"

;LangString message_checkbox_pats ${LANG_ENGLISH} "Delete PATs's directory"
LangString message_checkbox_pats ${LANG_SPANISH} "Eliminar directorio de PATs"

;LangString uninstalllink ${LANG_ENGLISH} "Uninstall.lnk"
LangString uninstalllink ${LANG_SPANISH} "Desinstalar $(application_name).lnk"

;LangString message_readme ${LANG_ENGLISH} "Installation succesful. Would you like to view $(readme_file)?"
LangString message_readme ${LANG_SPANISH} "Felicitaciones, la instalación ha concluído. ¿Desea ver el archivo $(readme_file)?"

;LangString readme_file ${LANG_ENGLISH} "readme.txt"
LangString readme_file ${LANG_SPANISH} "leame.txt"

;LangString license_file ${LANG_ENGLISH} "license.txt"
LangString license_file ${LANG_SPANISH} "licencia.txt"

;LangString changelog_file ${LANG_ENGLISH} "Changelog.txt"
LangString changelog_file ${LANG_SPANISH} "Cambios.txt"

;LangString message_pat_dir ${LANG_ENGLISH} ${PAT_DIR}
LangString message_pat_dir ${LANG_SPANISH} "${PAT_DIR}"

;LangString message_file_description ${LANG_ENGLISH} "PAT File"
LangString message_file_description ${LANG_SPANISH} "Archivo PAT"

;LangString message_warn_del_pat ${LANG_ENGLISH} "Warning: all the working files of users are gonna be deleted"
LangString message_warn_del_pat ${LANG_SPANISH} "Advertencia: se perderán todos los archivos de trabajo de los usuarios."

;LangString data_dir_installation ${LANG_ENGLISH} ": User PAT files directory"
LangString data_dir_installation ${LANG_SPANISH} ": Directorio de usuario"

;LangString data_dir_sellection_text ${LANG_ENGLISH} "Select PAT folder."
LangString data_dir_sellection_text ${LANG_SPANISH} "$(application_name) utiliza un directorio para almacenar la información de los Productos Autocontenidos Temáticos. Este directorio debe poseer suficiente espacio. El directorio seleccionado a continuación será el utilizado por la aplicación para guardar los archivos del usuario."

;LangString message_move_pat_dir ${LANG_ENGLISH} "Previous PAT directory found. Would you like to move the data found to the new directory?"
LangString message_move_pat_dir ${LANG_SPANISH} "Se encontró un directorio de PATs previo. ¿Desea mover los datos preexistentes al nuevo directorio?"
;--------------------------------
; The name of the installer
Name $(application_name)

; The file to write
OutFile "${INSTALLER_FILE}"

; Cuando se generen los iconos tener en cuenta la restriccion de que deben
; coincidir,  el icono  del instalador y el del desinstalador, en tamanio de
; bytes y pixel.
;Icon ${INSTALLER_ICON}
;UninstallIcon ${UNINSTALLER_ICON}

CRCCheck on
; The default installation directory
InstallDir "$PROGRAMFILES\$(application_dir)"

AllowRootDirInstall true

; Archivo de licencia.
LicenseData ${SRC_DIR}Licencia.txt

Var patDataDir

;--------------------------------
; P?ginas del instalador
Page license
Page directory
PageEx directory
  DirVar $patDataDir
  Caption $(data_dir_installation)
  DirText $(data_dir_sellection_text)
PageExEnd
Page instfiles

; P?ginas del desinstalador
UninstPage uninstConfirm
UninstPage custom un.CreateCustomPage un.LeaveCustomPage
UninstPage instfiles
;--------------------------------

Var uninstallDir ; Variable para obtener el directorio del desinstalador
Function .onInit
  Var /GLOBAL switch_overwrite
  StrCpy $switch_overwrite 0
  StrCpy $patDataDir "${USR_DIR}"
	; intento leer la clave del registro, para obtener el path del desinstalador.
   ClearErrors
   ReadRegStr $uninstallDir HKLM "${REG_KEY_2MP}\Uninstall" "UninstallString"
   ifErrors OnError Uninstall

Uninstall:
   MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION $(uninstall_message) IDOK +2
      Abort

   ClearErrors
   ; el parametro _? hace que ExecWait ejecute el desinstalador frenando las instrucciones del script hasta que termine
	; el desinstalador.
   ReadRegStr $INSTDIR HKLM "${REG_KEY_2MP}\Uninstall" "InstallLocation"
   ExecWait '"$uninstallDir" _?=$INSTDIR'
   IfErrors NoRemoveUninstaller Done

OnError:
   ; si hubo error de lectura del registro puede ser xq la clave no existe => uninstallDir==""
   StrCmp $uninstallDir "" Done
   Abort ; cualquier otro error aborto la desinstalacion

;si hubo errores en la desinstalacion aborto la ejecucion.
NoRemoveUninstaller:
Abort

Done:
   MessageBox MB_OKCANCEL|MB_ICONQUESTION $(message) IDOK Continue
      Abort

Continue:
FunctionEnd


Function .onInstSuccess
	MessageBox MB_YESNO $(message_readme) IDNO NoReadme
		Exec "notepad.exe $INSTDIR\$(readme_file)"
NoReadme:
FunctionEnd

; declaro variables utilizadas en funciones custom para crear controles
Var dialogCtrl
Var labelTitleCtrl
Var labelDirPatCtrl
Var textDirPatCtrl
Var checkboxCtrl
Var checkboxState
Var labelWarnCtrl

; funcion para crear pagina personalizada.
Function un.CreateCustomPage
	; valido que exista el directorio de pats del usuario. Solo si existe muestro la pagina con checkbox para desinstalar directorio de pats.
	ReadRegStr $patDataDir HKLM "${REG_KEY_2MP}" "PatDirectory"
	StrCmp $patDataDir "" set_directory 0
   ${DirState} "$patDataDir" $0
	IntCmp $0 0 found_directory not_found_directory found_directory
found_directory:
	GoTo Done
not_found_directory:
	Return
set_directory:
	StrCpy $patDataDir "${USR_DIR}"

Done:
   ; creo pagina personalizada.
   nsDialogs::Create /NOUNLOAD 1018
   Pop $dialogCtrl

   ; verifico que se haya creado sin errores.
   ${If} $dialogCtrl == error
      Abort
   ${EndIf}

   ; creo label con mensaje para usuario.
   ${NSD_CreateLabel} 0u 0u 100% 24u $(message_delete_usr_data)
   Pop $labelTitleCtrl

   ${NSD_CreateLabel} 0u 50u 50u 12u $(message_delete_pats_dir)
   Pop $labelDirPatCtrl

   ; creo una text deshabilitada para mostrar el path de la carpeta de PATs a eliminar.
   ${NSD_CreateText} 50u 48u 80% 12u "$patDataDir"
   Pop $textDirPatCtrl
   EnableWindow $textDirPatCtrl 0

	; creo label con mensaje de advertencia para el usuario.
   ${NSD_CreateLabel} 50u -30u 80% 48u $(message_warn_del_pat)
   Pop $labelWarnCtrl
	ShowWindow $labelWarnCtrl ${SW_HIDE}

   ; creo checkbox para saber si hay que eliminar carpeta de PATs.
   ${NSD_CreateCheckbox} 0u -50u 100% 8u $(message_checkbox_pats)
   Pop $checkboxCtrl
	${NSD_OnClick} $checkboxCtrl un.OnCheckbox

   nsDialogs::Show

Continue:
FunctionEnd

Function un.LeaveCustomPage
   ; obtengo el valor del checkbox
   ${NSD_GetState} $checkboxCtrl $checkboxState
FunctionEnd

; funcion que responde al checkeo del checkbox.
Function un.OnCheckbox
	Pop $checkboxCtrl
	${NSD_GetState} $checkboxCtrl $checkboxState
	${If} $checkboxState == ${BST_CHECKED}
      ShowWindow $labelWarnCtrl ${SW_SHOW}
	${Else}
	   ShowWindow $labelWarnCtrl ${SW_HIDE}
   ${EndIf}
FunctionEnd
;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important
   	; Le pido que reserve 100Mb m?s de los necesarios para los archivos
	; que trae el instalador
	AddSize 100000
	; Set output path to the installation directory.
	SetOutPath $INSTDIR

    ; Si el directorio elegido es distinto del default y el default existe, lo mueve
	${If} $patDataDir != "${USR_DIR}"
       ${DirState} "${USR_DIR}" $0
	   IntCmp $0 0 found_directory not_found_directory found_directory
found_directory:
	   MessageBox MB_YESNO $(message_move_pat_dir) IDNO NoMovePats
	   ${MoveDir} "${USR_DIR}" "$patDataDir" "*.*"
NoMovePats:
not_found_directory:
    ${EndIf}


	
   ; Put file there
   File ${SRC_DIR}${EXEFILE}
   ; Copio la configuracion
   File ${SRC_DIR}configuracion.xml
   ; Copio los datos, ignoro los directorios del svn
   File /r /x .svn ${SRC_DIR}data
   ; Copio el archivo leame
   File /nonfatal ${SRC_DIR}readme.txt
   File /nonfatal ${SRC_DIR}leame.txt

   ; Copio el archivo licencia
   File /nonfatal ${SRC_DIR}licencia.txt
   File /nonfatal ${SRC_DIR}license.txt

   ; Copio el archivo de cambios
   File /nonfatal ${SRC_DIR}Cambios.txt
   File /nonfatal ${SRC_DIR}Changelog.txt

   ; dlls de soporte
   File ${LIBTIFF_DLL_NAME}
   File ${LIBPNG_DLL_NAME}
   File ${LIBPROJ_DLL_NAME}
   File ${GDAL_DLL_NAME}
   File ${GEOS_DLL_NAME}
   File ${LIBGEOS_DLL_NAME}
   File ${KML_DLL_NAME}
   File ${LIBCURL_DLL_NAME}
   File ${LIBEXPAT_DLL_NAME}
   File ${LIBGEOTIFF_DLL_NAME}
   File ${LIBHDF5_DLL_NAME}
   File ${LIBJPEG_DLL_NAME}
   File ${LIBTIFFXX_DLL_NAME}
   File ${LIBZ_DLL_NAME}
   File ${LIBGCC_DLL_NAME}
   File ${LIBMUPARSER_DLL_NAME}
   File ${MINIZIP_DLL_NAME}
   File ${URI_DLL_NAME}
   File ${KMLDOM_DLL_NAME}
   File ${KMLCONV_DLL_NAME}
   File ${KMLENG_DLL_NAME}
   File ${KMLREG_DLL_NAME}

!if ${BUILD_ENVIRONMENT} < 20000
   File ${LIBPNG3_DLL_NAME}
!else if ${BUILD_ENVIRONMENT} < 30000
   File ${LIBWINPTHREAD_DLL_NAME}
   File ${LIBSTDCPP_DLL_NAME}
   File ${LIBSQLITE3_DLL_NAME}
   File ${LIBOPENJP2_DLL_NAME}
!endif

   ; dlls wx
   File ${WX_DLLS_DIR}wxbase28_gcc.dll
   File ${WX_DLLS_DIR}wxbase28_xml_gcc.dll
   File ${WX_DLLS_DIR}wxmsw28_adv_gcc.dll
   File ${WX_DLLS_DIR}wxmsw28_core_gcc.dll
   File ${WX_DLLS_DIR}wxmsw28_gl_gcc.dll
   File ${WX_DLLS_DIR}wxmsw28_html_gcc.dll
   File ${WX_DLLS_DIR}wxmsw28_xrc_gcc.dll
   ; suri
   !ifdef SURI_DLL
      File ${SURI_DLL}
   !endif
   
   ; Bitmaps
   ; Incluyo el icono de los archivos PAT.
   File ${PAT_ICON_PATH}

	; creo el desinstalador
	WriteUninstaller .\${UNINSTALLER_NAME}
	; renombro el ejecutable
	Rename "$INSTDIR\${EXEFILE}" "$INSTDIR\$(application_name).exe"
	
	CreateShortcut "$DESKTOP\$(application_name).lnk" "$INSTDIR\$(application_name).exe"

	CreateDirectory "$SMPROGRAMS\$(application_dir)"
	CreateShortcut "$SMPROGRAMS\$(application_dir)\$(application_name).lnk" "$INSTDIR\$(application_name).exe"
	CreateShortcut "$SMPROGRAMS\$(application_dir)\$(uninstalllink)" "$INSTDIR\${UNINSTALLER_NAME}"
	IfFileExists "$INSTDIR\$(license_file)" 0 +2
		CreateShortcut "$SMPROGRAMS\$(application_dir)\$(license_file).lnk" "$INSTDIR\$(license_file)"
   IfFileExists "$INSTDIR\$(readme_file)" 0 +2
      CreateShortcut "$SMPROGRAMS\$(application_dir)\$(readme_file).lnk" "$INSTDIR\$(readme_file)"
   IfFileExists "$INSTDIR\$(changelog_file)" 0 +2
      CreateShortcut "$SMPROGRAMS\$(application_dir)\$(changelog_file).lnk" "$INSTDIR\$(changelog_file)"
		

  ; creo clave en el registro de windows.
   WriteRegStr HKLM "${REG_KEY_2MP}\Uninstall" "DisplayName" "${APP_NAME}"
   WriteRegStr HKLM "${REG_KEY_2MP}\Uninstall" "UninstallString" "$INSTDIR\${UNINSTALLER_NAME}"
   WriteRegStr HKLM "${REG_KEY_2MP}\Uninstall" "InstallLocation" "$INSTDIR"
   WriteRegStr HKLM "${REG_KEY_2MP}\Uninstall" "DisplayVersion" "${VERSION}"

   ; asorio la extension .pat a la aplicacion.
	WriteRegStr HKCR "${FILE_EXT}" "" "$(message_file_description)"
	WriteRegStr HKCR "$(message_file_description)" "" "$(message_file_description)"
   WriteRegStr HKCR "$(message_file_description)\DefaultIcon" "" "$INSTDIR\${PAT_ICON},0"
	WriteRegStr HKCR "$(message_file_description)\shell" "" "open"
	WriteRegStr HKCR "$(message_file_description)\shell\edit" "" "Edit $(message_file_description)"
	WriteRegStr HKCR "$(message_file_description)\shell\edit\command" "" '"$INSTDIR\$(application_name).exe" "%1"'
	WriteRegStr HKCR "$(message_file_description)\shell\open" "" ""
	WriteRegStr HKCR "$(message_file_description)\shell\open\command" "" '"$INSTDIR\$(application_name).exe" "%1"'
	
	; cambio opciones de configuracion
	${StrReplace} "$patDataDir" "\" "/"
	var /GLOBAL newPatDataDir
	StrCpy $newPatDataDir $0
	${If} $patDataDir != "${USR_DIR}"
		WriteRegStr HKLM "${REG_KEY_2MP}" "PatDirectory" "$patDataDir"
		${ReplaceBetween} "<pat_save_dir>" "</pat_save_dir>" "$newPatDataDir/pats/usuario/" "$INSTDIR\configuracion.xml"
		${ReplaceBetween} "<pat_install_dir>" "</pat_install_dir>" "$newPatDataDir/pats/instalados/" "$INSTDIR\configuracion.xml"
	${EndIf}

	; invoco la funcion RefreshShellIcons para que se actualizen los iconos de los PATs. Fuerza una lectura del registro de WIN  y evito que se tenga que reiniciar la pc.
	${RefreshShellIcons}
SectionEnd ; end the section

Section "Uninstall"
	Delete $INSTDIR\${UNINSTALLER_NAME} ; delete self (see explanation below why this works)

	; Borra el directorio de instalacion (esto es peligroso)
	RMDir /r "$INSTDIR"

  ; Verifico valor del checkbox para saber si debo eliminar carpeta de PATS
  ${If} $checkboxState == ${BST_CHECKED}
		var /GLOBAL usrDir
		ReadRegStr $usrDir HKLM "${REG_KEY_2MP}" "PatDirectory"
		${If} $usrDir == ""
			StrCpy $usrDir "${USR_DIR}"
		${EndIf}
		; elimino todo el directorio 2Mp que se encuentra dentro del directorio de datos del usuario.
		SetOutPath "$PROGRAMFILES"
		DetailPrint "Eliminar carpeta de PATs: $usrDir"
		RMDir /r "$usrDir\"
  ${EndIf}

	; Borra los links del menu START
	RMDir /r "$SMPROGRAMS\$(application_dir)"
	Delete "$DESKTOP\$(application_name).lnk"

   ; elimino clave en el registro de windows.
   DeleteRegKey HKLM "${SUR_REG_KEY_ROOT}"

   ; desasorio la extension .pat de la aplicacion.
   DeleteRegKey HKCR "${FILE_EXT}"
   DeleteRegKey HKCR "$(message_file_description)"
	${RefreshShellIcons}
SectionEnd




