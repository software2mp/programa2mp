# Configuracion base para el proyecto.
project (Programa2Mp)

cmake_minimum_required(VERSION 2.6)
SET(MAJOR_VERSION 3)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 0)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/lib/suri/tools)

IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
	add_subdirectory (lib/)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
add_subdirectory (src/)


# Configuracion para CPack
SET(PACKAGE_SOURCE_DIR ${CMAKE_BINARY_DIR}/package)

# Instala el binario de Programa 2Mp
INSTALL(PROGRAMS ${PACKAGE_SOURCE_DIR}/programa2mp.sh ${PACKAGE_SOURCE_DIR}/Programa2Mp DESTINATION bin)

INSTALL(DIRECTORY ${PACKAGE_SOURCE_DIR}/data DESTINATION /usr/local/share/${CMAKE_PROJECT_NAME})

INSTALL(DIRECTORY ${PACKAGE_SOURCE_DIR}/lib DESTINATION /usr/share/lib/${CMAKE_PROJECT_NAME})

INSTALL(FILES ${PACKAGE_SOURCE_DIR}/Programa2Mp.desktop ${PACKAGE_SOURCE_DIR}/hicolor/48x48/apps/2mp.png DESTINATION /usr/share/applications/)


SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Programa 2Mp")
SET(CPACK_PACKAGE_DESCRIPTION "En el marco del Plan Espacial Nacional 2004-2015 se crea el 'Programa de Entrenamiento Satelital para niños y jóvenes 2Mp' con el objetivo de acercar la tecnología satelital a 2 Millones de pibes. A través del desarrollo del Programa 2Mp se busca que los alumnos a partir de 8 años de las escuelas de nuestro país conozcan, tengan acceso y utilicen la información de origen satelital, y que puedan aplicarla en lo sucesivo a las actividades que desarrollan en el ámbito de su vida cotidiana.")

SET(CPACK_PACKAGE_VENDOR "CONAE")
SET(CPACK_PACKAGE_CONTACT "programa2mp@conae.gov.ar")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/src/Leame.txt)
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/src/Licencia.txt)
SET(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/lib/resources/bitmaps/icon-Application-48.png)

# Dependencias
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libjpeg62, librtmp0")
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://2mp.conae.gov.ar/")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "SUR Emprendimientos Tecnológicos <contacto@suremptec.com.ar>")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "En el marco del Plan Espacial Nacional 2004-2015 se crea el 'Programa de Entrenamiento Satelital para niños y jóvenes 2Mp' con el objetivo de acercar la tecnología satelital a 2 Millones de pibes. A través del desarrollo del Programa 2Mp se busca que los alumnos a partir de 8 años de las escuelas de nuestro país conozcan, tengan acceso y utilicen la información de origen satelital, y que puedan aplicarla en lo sucesivo a las actividades que desarrollan en el ámbito de su vida cotidiana.\n")
SET(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

INCLUDE(CPack)

