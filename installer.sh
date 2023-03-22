#!/bin/bash

set -e
set -u

# Directorio donde se hace el paquete de instalacion
PACKAGEDIR=package/
# Directorio (fuera del arbol de fuentes) donde guarda los archivos
# de autopackage
AUTOPACKAGELOCALDIR=$HOME/opt/
# Directorio de origen de datos
SRCDIR=src/
# nombre deseado (sin espacios)
PROGRAMNAME="Programa 2Mp"
# nombre del ejecutable que queda en el directorio de compilacion
# debe coincidir con el definido en CMakeLists.txt
EXECUTABLENAME=Programa2Mp
# bibliotecas a incluir en el paquete
INCLUDEDLIBS="wx|gdal|hdf|mfhdf|proj|jasper|geos|geotiff|curl|expat|kml|muparser|suri|minizip|uriparser|sqlite|openjp2|${AUTOPACKAGELOCALDIR}"

# nombre (deseado) del programa en minuscula
PROGRAMNAMELOWERCASE=$(echo ${EXECUTABLENAME} | tr "[:upper:]" "[:lower:]")
# directorio dentro de la instalacion para los instaladores de autopackage
AUTOPACKAGES="${PACKAGEDIR}"autopackage/packages/
# extension mime
MIMEEXTENSION=pat
# nombre del icono
ICON="2mp.png"
MIMEICON="${MIMEEXTENSION}.png"
# nombre del script de lanzamiento
LAUNCHSCRIPT="${PROGRAMNAMELOWERCASE}.sh"
# desktop file
DESKTOPNAME="${EXECUTABLENAME}.desktop"

sed_file() {
	FILENAME="$1"
	shift
	EXPR="$1"
	shift
	VERSION="$1"
	shift
	if [ ! -f "${FILENAME}" ] ; then
		echo "No existe ${FILENAME}"
		exit -1
	fi
	echo -n "Actualizando ${FILENAME} con ${VERSION}" ""
	rm -f /tmp/tmp.expr
	mv "${FILENAME}" /tmp/tmp.expr
	sed -r "0,/${EXPR}/s//\1${VERSION}\2/g" /tmp/tmp.expr > "${FILENAME}"
	! cmp -s /tmp/tmp.expr "${FILENAME}" || return 1
	rm -f /tmp/tmp.expr
	echo "[ OK ]"
}

replace_variables(){
	FILE_ID=1
	REPLACEMENTS=(
		"LinuxShortcut.desktop" "(^Exec=).*($)" "${LAUNCHSCRIPT} %f" 1									\
		"LinuxShortcut.desktop" "(^Icon=).*($)" ${ICON} 1													\
		"LinuxShortcut.desktop" "(^TryExec=).*($)" "${EXECUTABLENAME}" 1								\
		"LinuxShortcut.desktop" "(^Name.*=\s*)($)" "${PROGRAMNAME}" 1									\
		"LinuxShortcut.desktop" "^(MimeType=application\/).*(;^)" "${MIMEEXTENSION}" 1			\
		"default.apspec" "(^RootName: \@suremptec\.com\/).*(:.*$)"	"${PROGRAMNAMELOWERCASE}" 1\
		"default.apspec" "(^DisplayName:).*($)" " ${PROGRAMNAME}" 1										\
		"default.apspec" "(^ShortName:).*($)"	" ${PROGRAMNAMELOWERCASE}" 1							\
		"default.apspec" "(^installIcon).*($)" " hicolor" 1												\
		"default.apspec" '(^copyFiles).*($)' ' data "$PREFIX"\/share\/'${EXECUTABLENAME} 1	\
		"default.apspec" '(^installExe).*($)' " ${EXECUTABLENAME} ${LAUNCHSCRIPT}" 1				\
		"default.apspec" '(^installMenuItem ".+)".*("$)' "\"${DESKTOPNAME}" 1						\
		"application.xml" '(\<apkg:icon>).*(<\/apkg:icon\>)' "${MIMEICON}" 1							\
		"application.xml" '(\<icon name=").*(")' "${MIMEICON}" 1											\
		"application.xml" '(\<glob pattern="\*\.).*(")' "${MIMEEXTENSION}" 1							\
		"application.xml" '(\<mime-type type="application\/).*(")' "${MIMEEXTENSION}" 1			\
	)
	for ((i=0;i<${#REPLACEMENTS[*]};i+=1)); do
		FILE=${REPLACEMENTS[${i}]}
		let "i = $i + 1"
		REGEXP=${REPLACEMENTS[${i}]}
		let "i = $i + 1"
		FILL=${REPLACEMENTS[${i}]}
		let "i = $i + 1"
		if [ ${REPLACEMENTS[${i}]} == ${FILE_ID} ] ; then
			sed_file "${FILE}" "${REGEXP}" "${FILL}" || echo "[ SIN MODIFICAR ]"
		fi
	done
}

download_makepackage(){
	PACKAGENAME="$1"
	shift
	PACKAGESOURCE="$1"
	shift
	PACKAGEURL="$1"
	shift
	# si no existe el local, lo bajo
	if [ ! -f ${PACKAGESOURCE}/${PACKAGENAME} ] ; then
		mkdir -p ${PACKAGESOURCE}
		echo -n " * bajando - ${PACKAGENAME} -> ${PACKAGESOURCE}/${PACKAGENAME}"
		set +e
		set +u
		wget "${PACKAGEURL}" -O "${PACKAGESOURCE}/${PACKAGENAME}" -o wget-error.log
		if [ ! $? ] ; then
			echo " [ FALLA ]"
			cat wget-error.log
			rm "${PACKAGESOURCE}/${PACKAGENAME}"
			else
			echo " [ok]"
		fi
		set -e
		set -u
		rm -f wget-error.log
	fi
}

copy_files(){
	for ((i=0;i<$(eval echo \${#$1[*]});i+=1)); do
		SRC=$(eval echo \${$1[${i}]})
		let "i = $i + 1"
		DEST=$(eval echo \${$1[${i}]})
		echo -n " * Copiando ${SRC} -> ${DEST}"
		if [ -d ${SRC} ] ; then
			cp -R "${SRC}" "${DEST}"
		else
			DESTDIR=${DEST%/*}
			if [ ! -d ${DESTDIR} ] ; then
				mkdir -p "${DESTDIR}"
			fi
			if [ -f "${SRC}" ] ; then
			cp "${SRC}" "${DEST}"
			else
				echo " [ No se encontro ${SRC} ]"
				continue
		fi		
		fi		
		echo " [ OK ]"
	done
}

echo -n LIMPIANDO DIRECTORIO ${PACKAGEDIR}
rm -Rf "${PACKAGEDIR}" && echo " [ OK ]"
mkdir -p "${PACKAGEDIR}"/lib

# Reemplazo los datos
replace_variables

echo COPIANDO ARCHIVOS
# copio los paquetes, si no existen, los baja
download_makepackage autopackage.tar.bz2 ${AUTOPACKAGELOCALDIR} https://web.archive.org/web/20100922105211/http://autopackage.googlecode.com/files/autopackage-1.4.2-x86.tar.bz2
download_makepackage autopackage-gtk-1.4.2.package ${AUTOPACKAGELOCALDIR} https://web.archive.org/web/20100922105211/http://autopackage.googlecode.com/files/autopackage-gtk-1.4.2.package

ARRAY=("${SRCDIR}${EXECUTABLENAME}" "${PACKAGEDIR}"									\
		"default.apspec" "${PACKAGEDIR}"														\
		"${SRCDIR}data" "${PACKAGEDIR}"														\
		"lib/resources/bitmaps/icon-Application-48.png" "${PACKAGEDIR}hicolor/48x48/apps/${ICON}"			\
		"lib/resources/bitmaps/icon-PATFile-48.png" "${PACKAGEDIR}hicolor/48x48/mimetypes/${MIMEICON}"	\
		"LinuxShortcut.desktop" "${PACKAGEDIR}${DESKTOPNAME}"							\
		"${SRCDIR}configuracion.xml" "${PACKAGEDIR}data/"								\
		${AUTOPACKAGELOCALDIR}autopackage.tar.bz2 "${AUTOPACKAGES}"					\
		${AUTOPACKAGELOCALDIR}autopackage-gtk-1.4.2.package "${AUTOPACKAGES}"	\
		application.xml "${PACKAGEDIR}application-${MIMEEXTENSION}.xml")
copy_files ARRAY

# genera el script bash de lanzamiento
echo -n " * Generando script de inicio en ${PACKAGEDIR}${PROGRAMNAMELOWERCASE}"
echo -e \#\!/bin/bash \\n \\nCALLNAME=\$0 \\nCALLDIR=\$\{CALLNAME\%/*\} \\nexport LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\"\$CALLDIR/../lib\" \\n\"\$CALLDIR\"/"${EXECUTABLENAME}" '"$1"' > "${PACKAGEDIR}${LAUNCHSCRIPT}"
echo " [ OK ]"

# hago ejecutables al binario y al script
chmod a+x "${PACKAGEDIR}${EXECUTABLENAME}"
chmod a+x "${PACKAGEDIR}${LAUNCHSCRIPT}"

echo "ACHIVOS COPIADOS [ OK ]"

echo COPIANDO DEPENDENCIAS DINAMICAS
j=0
ARRAY=""
# armo el array con los archivos .so necesarios
for i in $(ldd "${PACKAGEDIR}${EXECUTABLENAME}" | grep -E "${INCLUDEDLIBS}" | cut -d ' ' -f 3); do
	ARRAY[${j}]="${i}"
	let "j = $j + 1"
	ARRAY[${j}]="${PACKAGEDIR}"lib
	let "j = $j + 1"
done
copy_files ARRAY

echo -n ELIMINANDO DIRECTORIOS SVN
for hidden_file in $(find "${PACKAGEDIR}" -name ".*") ; do
	rm -Rf ${hidden_file}
done
echo " [ OK ]"

if [ `which makepackage` ] ; then
	echo -n "GENERANDO INSTALADOR" ; set -x
	cd "${PACKAGEDIR}"
	makepackage default.apspec > package.log 2>&1
	if [ ! $? ] ; then
		echo " [ FALLA ]" ; cat package.log ; rm package.log ; exit 1
	fi
	echo " [ OK ]"
	cat package.log
	rm package.log
	cd ..
	cp "${PACKAGEDIR}"/*.package .
fi

echo FIN
exit 0