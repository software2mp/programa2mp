#ifndef MESSAGES_H_
#define MESSAGES_H_

/**
 * Prefijos de los define para las distintas funciones de cada constante:
 *
 *  tooltip		: Mensajes emergentes de cualquier elemento de pantalla
 *  content		: contenido default de un campo (Ej: texto default de un wxTextBox, Opciones de un wxComboBox, etc)
 *  label		: texto invariante que debe mostrado en pantalla (Ej: texto de un wxStaticBox)
 *  caption		: Titulo de ventana/dialogo
 *  filter		: Filtro de extensiones para los dialogos (No se traduce, solo para reusarlo)
 *  message		: Todo mensaje al usuario que sera mostrado mediante un SHOW_XXX()
 *  question	: Pregunta al usuario
 *  menu			: Entrada de menu
 *  format		: String de formateo de datos
 *  filename	: Nombre de archivo
 *  extension	: Extension (con punto)
 *  name			: Nombre de algo (por ejemplo Punto)
 *
 */

//Tooltips----------------------------------------------------------------------
//Caption-----------------------------------------------------------------------
#define caption_SELECT_GENERIC_IMAGE "Seleccionar imagen"
#define caption_SELECT_PAT_DIR "Seleccionar el directorio para guardar el archivo PAT comprimido"
#define caption_SELECT_PAT_PREVIEW "Seleccionar vista previa del PAT"
#define caption_SELECT_TEXTURE "Seleccionar imagen de textura para el terreno"
#define caption_SELECT_ELEMENT "Seleccionar el elemento para agregar"

#define caption_INSTALL_PAT "Instalar PAT"
#define caption_PAT_MANAGER "Administracion de PATs"
#define caption_BLANK_PAT "Nuevo PAT"
#define caption_SAVE_PAT "Guardar PAT"
#define caption_PAT_PROPERTIES "Descripcion de PAT"
#define caption_PAT_TOOLBAR "Herramientas de PAT"
#define caption_PAT_INFORMATION "Informacion de PAT"
#define caption_HTML_VIEWER "Visualizador de HTML"
#define caption_PRINT "Imprimir"
#define caption_HELP_s "Ayuda: %s"

//Contextos
#define caption_RASTER_CONTEXT "Imagenes" //migrar al xmlnames
#define caption_MAP_CONTEXT "Mapas"  //migrar al xmlnames
#define caption_TERRAIN_CONTEXT "Terrenos 3D" //migrar al xmlnames
#define caption_DOCUMENTS_CONTEXT "Documentos" //migrar al xmlnames
#define caption_PHOTOGRAPHS_CONTEXT "Fotografias" //migrar al xmlnames
#define caption_ACTIVITIES_CONTEXT "Actividades" //migrar al xmlnames
//Propiedaes
#define caption_PHOTO_PROPERTIES "Propiedades de fotografia"
#define caption_HOTLINK_PROPERTIES "Propiedades de hipervinculo"

//#define content_SELECT_PAT "Seleccione un PAT para obtener informacion"
#define content_NO_DESCRIPTION "Sin descripcion"
#define content_INVALID_DATE "Fecha invalida"
#define content_FALLBACK_DATE "Fecha invalida"
#define content_FALLBACK_AUTHOR "Anonimo"

//Extension---------------------------------------------------------------------
#define extension_DOT_PAT ".pat"
#define extension_PAT "pat"

//Filename----------------------------------------------------------------------
//#define filename_DESCRIPTION_XML "descripcion.xml"
//#define filename_CONFIGURATION_XML "configuracion.xml"

/** macros auxiliares */
#define JOIN_1(Parameter) Parameter
/** Concatena 2 parametros con un separador. */
#define JOIN_2(Parameter1, Parameter2, Separator) Parameter1 +                    \
   std::string(Separator) + Parameter2
/** Concatena 3 parametros con un separador. Utiliza JOIN_2 */
#define JOIN_3(Parameter1, Parameter2, Parameter3, Separator)                     \
   JOIN_2(JOIN_2(Parameter1, Parameter2, Separator), Parameter3,                  \
          Separator)
/** Concatena 4 parametros con un separador. Utiliza JOIN_3 */
#define JOIN_4(Parameter1, Parameter2, Parameter3, Parameter4, Separator)         \
   JOIN_2(JOIN_3(Parameter1, Parameter2, Parameter3, Separator),                  \
          Parameter4, Separator)
/** Concatena 5 parametros con un separador. Utiliza JOIN_4 */
#define JOIN_5(Parameter1, Parameter2, Parameter3, Parameter4, Parameter5,        \
               Separator)                                                         \
   JOIN_2(JOIN_4(Parameter1, Parameter2, Parameter3, Parameter4,                  \
                 Separator), Parameter5, Separator)
#define JOIN_6(Parameter1, Parameter2, Parameter3, Parameter4, Parameter5,        \
               Parameter6, Separator)                                             \
   JOIN_2(JOIN_5(Parameter1, Parameter2, Parameter3, Parameter4,                  \
                 Parameter5, Separator), Parameter6, Separator)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define FILTER_SEPARATOR "|"
#define EXTENSION_SEPARATOR ";"
#define BUILD_FILTER(Name, Filter) _(Name) + std::string(FILTER_SEPARATOR Filter)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#ifdef __WXMSW__
#  define filter_extension_ALL "*.*"
#  define filter_extension_BMP "*.bmp"
#  define filter_extension_JPG JOIN_3("*.jpg", "*.jpeg", "*.jpe",                 \
                                      EXTENSION_SEPARATOR)
#  define filter_extension_PNG "*.png"
#  define filter_extension_GIF "*.gif"
#  define filter_extension_TIF JOIN_2("*.tif", "*.tiff", EXTENSION_SEPARATOR)
#  define filter_extension_FST JOIN_4("*_hrf.fst", "*_htm.fst", "*_hpn.fst",      \
                                      "header.dat", EXTENSION_SEPARATOR)
#  define filter_extension_SHAPEFILE "*.shp"
#  define filter_extension_CEOS "VOL-*"
#  define filter_extension_HDF5 JOIN_2("*.hdf", "*.h5", EXTENSION_SEPARATOR)
#  define filter_extension_L1B "*.l1b"
#  define filter_extension_GPX "*.gpx"
#  define filter_extension_KML "*.kml"
#  define filter_extension_HTM JOIN_2("*.htm", "*.html", EXTENSION_SEPARATOR)
#  define filter_extension_XLS "*.xls"
#else
#  define filter_extension_ALL "*"
#  define filter_extension_BMP "*.[Bb][Mm][Pp]"
#  define filter_extension_JPG JOIN_3("*.[Jj][Pp][Gg]", "*.[Jj][Pp][Ee][Gg]",     \
                                      "*.[Jj][Pp][Ee]", EXTENSION_SEPARATOR)
#  define filter_extension_PNG "*.[Pp][Nn][Gg]"
#  define filter_extension_GIF "*.[Gg][Ii][Ff]"
#  define filter_extension_TIF JOIN_2("*.[Tt][Ii][Ff]", "*.[Tt][Ii][Ff][Ff]",     \
                                      EXTENSION_SEPARATOR)
#  define filter_extension_FST JOIN_4("*_[Hh][Rr][Ff].[Ff][Ss][Tt]",              \
                                      "*_[Hh][Tt][Mm].[Ff][Ss][Tt]",              \
                                      "*_[Hh][Pp][Nn].[Ff][Ss][Tt]",              \
                                      "*.[Dd][Aa][Tt]", EXTENSION_SEPARATOR)
#  define filter_extension_SHAPEFILE "*.[Ss][Hh][Pp]"
#  define filter_extension_CEOS "[Vv][Oo][Ll]-*"
#  define filter_extension_HDF5 JOIN_2("*.[Hh][Dd][Ff]", "*.[Hh]5",               \
                                      EXTENSION_SEPARATOR)
#  define filter_extension_L1B "*.[Ll]1[Bb]"
#  define filter_extension_GPX "*.[Gg][Pp][Xx]"
#  define filter_extension_KML "*.[Kk][Mm][Ll]"
#  define filter_extension_HTM JOIN_2("*.[Hh][Tt][Mm]", "*.[Hh][Tt][Mm][Ll]",     \
                                      EXTENSION_SEPARATOR)
#  define filter_extension_XLS "*.[Xx][Ll][Ss]"
#endif

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_extension_GEORREFERENCED_IMAGE JOIN_2(filter_extension_TIF,        \
                                                     filter_extension_FST,        \
                                                     EXTENSION_SEPARATOR)
#define filter_extension_COMMON_IMAGE JOIN_5(filter_extension_BMP,                \
                                             filter_extension_JPG,                \
                                             filter_extension_PNG,                \
                                             filter_extension_GIF,                \
                                             filter_extension_TIF,                \
                                             EXTENSION_SEPARATOR)
#define filter_extension_VIEWER_3D JOIN_2(filter_extension_TERRAIN,               \
                                          filter_extension_VECTOR,                \
                                          EXTENSION_SEPARATOR)
#define filter_extension_TERRAIN filter_extension_TIF
#define filter_extension_TEXTURE filter_extension_GEORREFERENCED_IMAGE
#define filter_extension_VECTOR JOIN_3(filter_extension_SHAPEFILE,                \
                                       filter_extension_GPX,                      \
                                       filter_extension_KML,                      \
                                       EXTENSION_SEPARATOR)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
/** Filtros--------------------------------------------------------------------- */
#define filter_ALL_FILES BUILD_FILTER_WITH_EXT(name_ALL_FILES, extension_ALL,     \
                                               filter_extension_ALL)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
//Filter------------------------------------------------------------------------
#define filter_extension_COMMON_IMAGE_FORMATS_LOAD JOIN_5(filter_extension_BMP,      \
                                                filter_extension_PNG,                \
                                                filter_extension_JPG,                \
                                                filter_extension_TIF,                \
                                                filter_extension_GIF,                \
                                                EXTENSION_SEPARATOR)

#define filter_COMMON_IMAGE_FORMATS_LOAD BUILD_FILTER(name_COMMON_IMAGE_FILES,       \
                                   filter_extension_COMMON_IMAGE_FORMATS_LOAD)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
//Context Filter----------------------------------------------------------------
#define filter_extension_CONTEXT_IMAGE_FILES_LOAD JOIN_6(filter_extension_BMP,      \
                                               filter_extension_PNG,                \
                                               filter_extension_JPG,                \
                                               filter_extension_TIF,                \
                                               filter_extension_GIF,                \
                                               filter_extension_SHAPEFILE,          \
                                               EXTENSION_SEPARATOR)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_CONTEXT_IMAGE_FILES_LOAD BUILD_FILTER("Archivos Soportados",         \
                                   filter_extension_CONTEXT_IMAGE_FILES_LOAD)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_extension_CONTEXT_MAP_FILES_LOAD JOIN_6(filter_extension_BMP,        \
                                               filter_extension_PNG,                \
                                               filter_extension_JPG,                \
                                               filter_extension_TIF,                \
                                               filter_extension_GIF,                \
                                               filter_extension_SHAPEFILE,          \
                                               EXTENSION_SEPARATOR)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_CONTEXT_MAP_FILES_LOAD BUILD_FILTER("Archivos Soportados",           \
                                   filter_extension_CONTEXT_MAP_FILES_LOAD)

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_CONTEXT_TERRAIN_FILES_LOAD "Archivos Soportados|*.tif;*.tiff;*.shp"

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_CONTEXT_DOCUMENT_FILES_LOAD "Archivos de Documentos|*.html;*.htm;*.pdf"

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_CONTEXT_PHOTO_FILES_LOAD "Archivos de Fotografias|*.bmp;*.png;*.jpg;*.jpeg;*.jpe;*.gif"

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_CONTEXT_ACTIVITY_FILES_LOAD "Archivos de Actividades|*.html;*.htm"

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_ALL_FILES_LOAD "Todos los Archivos (*.*)|*.*"

// TODO(Sebastian - TCK# 848): Mover este codigo a la libreria
#define filter_FORMAT_SEPARATOR "|"

//Format------------------------------------------------------------------------
#define filter_PAT_FILES "Archivos PAT (*.pat)|*.pat;*.PAT" //No agregar M:
//Label-------------------------------------------------------------------------

//Meassure Tool
#define caption_DISTANCE "Medir distancia"
#define caption_AREA "Medir area"

//Menu--------------------------------------------------------------------------

//Message-----------------------------------------------------------------------

//Context
//#define message_EMPTY_CONTEXT "Error al agregar contextos: lista de contextos vacia."
//#define message_CONTEXT_ADD_ERROR "Error al crear archivo descripcion.xml: no se pudo agregar contextos."
//#define message_UNNAMED_CONTEXT "Contexto sin nombre."

//Dir
#define message_OPEN_DIR_ERROR_s "Error al intentar abrir directorio: %s."
//#define message_LIST_DIR_ERROR "N/D"
#define message_NOT_A_VALID_DIR "%s no es un directorio valido."
#define message_DELETE_DIR_ERROR_s "Error al eliminar directorio: %s."
//#define message_CREATE_DIR_ERROR_2_s "Path invalido. No se puede crear directorio: %s."
//#define message_DIR_NOT_FOUND "No existe o no se pudo crear directorio con templates de datos."
//#define message_TEMP_DIR_CREATION_ERROR "No se pudo generar directorio temporal."
#define message_HOTLINK_DIR_CREATION_ERROR "No se pudo generar directorio temporal para hipervinculos."
#define message_VECTOR_DIR_CREATION_ERROR "No se pudo generar directorio temporal para vectores."

//Element
#define message_COPY_ELEMENT_ERROR "Error al copiar el elemento."
#define message_INVALID_OR_DUPLICATED_CONTEXT_ELEMENT_ss "El elemento %s no es de un tipo valido o ya existe con ese nombre de archivo en el contexto %s."
#define message_CLOSE_ELEMENTS_ERROR "Error al intentar cerrar elementos."

//File
//#define message_OPEN_FILE_ERROR "N/D"
//#define message_READ_FILE_ERROR "N/D"
#define message_DELETE_FILE_ERROR_s "Error al intentar eliminar archivo: %s. Verifique que exista y que no este siendo usado."
#define message_MISSING_FILE "Archivo inexistente."
#define message_FILE_NOT_FOUND_s "No existe el archivo: %s."
//#define message_FILE_IS_OPENED "Archivo ya abierto"
#define message_TEMP_FILE_CREATION_ERROR "Falla creacion del archivo temporal."
#define message_HTML_FILES_NOT_FOUND "No se pudo obtener archivos para desplegar informacion."

//General
#define message_MISSING_TYPE_PROPERTY "Sin propiedad <tipo>."
//#define message_NULL_LIST "Lista de elementos invalida."
//#define message_INSERT_ERROR_s "No se pudo insertar %s."
#define message_MISSING_TEMPLATE "No se encontro el template."
//#define message_SHOWING_INFO "Mostrando Info"
//#define message_PATPROPERTIES_REMOVE_ERROR "Error al intentar eliminar Part."
//#define message_NO_LEGENDS "No hay leyendas para dibujar."
//#define message_ERROR_CREATING_MAIN_TOOL "No se creo la herramienta principal."
//#define message_MISSING_LAYER_d "No se encuentra la capa: %d."
//#define message_MISSING_FEATURE "No se encuentra nodo caracteristicas."
//#define message_MISSING_FEATURES "No se pudieron obtener valores del nodo caracteristicas."
//#define message_INFORMATION_FILES_MISSING "No se pudo obtener archivos para desplegar informacion."
//#define message_SHOW_PAT_PROPERTIES_ERROR "Error al intentar agregar PatPropertiesHtmlViewerPart."
//#define message_SHOW_PAT_INFO_ERROR "Error al intentar agregar PatInfoPart."
#define message_INVALID_IMAGE "La imagen seleccionada no es una imagen valida."

//Hotlinks
//#define message_HOTLINK_COPY_ERROR "Error al intentar copiar hipervinculos."
//#define message_MISSING_HOTLINK "No se encuentra el nodo hipervinculo."
#define message_UPDATE_HOTLINKS_ERROR "Error al modificar hipervinculos."

//PATs
#define message_PAT_XML_GENERATION_ERROR "No se pudo generar archivo de configuracion del PAT."
//#define message_INSTALLED_PAT_SEARCH_ERROR "Error buscando PATs instalados."
#define message_PAT_XML_SAVE_ERROR "No se pudo salvar archivo de configuracion del PAT."
#define message_PAT_NAME_MISSING "Debe ingresar un nombre para el PAT."
#define message_SELECT_PAT "Seleccione un PAT de la lista."
//#define message_PAT_INFO_NODE_MISSING "No se pudo obtener nodo informacion."
//#define message_PAT_NODE_ERROR "Error al agregar contextos: nodo PAT vacio."
//#define message_PAT_ROOT_CREATE_ERROR "Error al crear archivo descripcion.xml: no se pudo crear raiz."
#define message_PAT_SUCCESFULL_GENERATION "Archivo generado con exito."
//#define message_PAT_DESCRIPTION_ERROR "Error al intentar obtener descripcion."
#define message_PAT_LOAD_ERROR "Error al cargar el PAT."
#define message_PAT_LOAD_ELEMENTS_ERROR "Los siguientes elementos no pudieron ser cargados:"
#define message_PAT_UNINSTALL_ERROR_s "Error al intentar desinstalar PAT: %s."
#define message_PAT_INSTALL_ERROR "Error al instalar PAT."
#define message_PAT_INSTALL_ERROR_DESCRIPTION_s "Error al instalar PAT. Motivo: %s."
#define message_PORPIETARY_PAT_EXISTS "Error al intentar crear PAT: ya existe un PAT propietario con el mismo nombre."
#define message_PAT_MISSING_FILE_s "Archivo %s del xml no se encuentra en el PAT."
//#define message_NO_COMPRESSED_PAT_s "El PAT %s no es un archivo PAT valido."
#define message_MISSING_PAT_DEPENDENCY "Dependencia de archivos insatisfecha en el PAT."
#define message_PAT_FILE_ADD_ERROR "Error agregando archivos al PAT."
#define message_PAT_CREATE_ERROR "Error al intentar crear PAT."
#define message_PAT_INVALID_TYPE "Archivo PAT invalido."
//#define message_INVALID_PAT_TYPE2 "Tipo de PAT invalido."
#define message_PAT_DIR_CREATION_ERROR "Error al intentar crear directorio de instalacion de PATs."
#define message_PAT_CONSISTENCY_ERROR "Inconsistencia entre archivos dentro del PAT y descripcion.xml."
//#define message_PAT_DIRECTORY_NAME_CONSISTENCY_ERROR "Nombre de directorio diferente a nodo informacion|nombre."
#define message_PAT_DESCRIPTION_EXTRACTION_ERROR "Error al descomprimir archivo descripcion.xml."
//#define message_PAT_DESCRIPTION_EXTRACTION_ERROR2 "Fallo extraccion del archivo descripcion.xml."
#define message_PAT_PREVIEW_EXTRACTION_ERROR "Fallo extraccion del archivo de vista previa."
//#define message_PAT_GENERATION_ERROR2 "PAT generado con errores."
#define message_PAT_GENERATION_ERROR_s "PAT generado con errores. Descripcion: %s."
#define message_PAT_INSTALLATION_SUCCESSFULL "Instalacion de PAT exitosa."
#define message_CANNOT_EDIT_INSTALLED_PAT "Solo se pueden editar PATs de usuario."
#define message_PAT_FORMAT_ERROR_s "El PAT: %s no es un archivo PAT comprimido."
#define message_MISSING_PREVIEW "No existe el archivo seleccionado para vista previa."
//#define message_PATINFO_REMOVE_ERROR "Error al intentar eliminar la informacion de PAT."
#define message_NO_FILES_IN_PAT "No se encontraron archivos en el PAT."
//#define message_CREATE_PAT_INSTALLATION_DIR_ERROR "No se pudo crear el directorio de instalacion de PATs."
//#define message_CREATE_PAT_INSTALLATION_DIR_ERROR2 "No se pudo crear el directorio de instalacion para el PAT: "
//#define message_CREATE_PAT_INSTALLATION_DIR_ERROR3 "No se pudo crear el directorio de instalacion para el PAT: %s."
#define message_COMPRESSED_PAT_GENERATION_ERROR "No se pudo generar el PAT comprimido. Motivo: %s."
#define message_PAT_ELEMENTS_NOT_FOUND "No se encontraron referencias a los elementos del PAT."
#define message_PAT_NOT_ENOUGH_SPACE "Espacio en disco insuficiente"
#define message_EMPTY_PAT_ERROR "No se puede generar un PAT sin elementos."
#define message_INCOMPATIBLE_PAT_VERSION "Version de PAT incompatible, descargar nueva version."

//Terrain
#define message_TERRAIN_MUST_BE_DISPLAYED "un terreno"
#define message_MUST_SELECT_TEXTURE  "Debe seleccionar una textura para el terreno."
#define message_WRONG_TYPE_VECTOR_ss "Error al intentar agregar vector: %s. El vector no es de tipo: %s."

//Generico
// se complementa con message_RASTER_IMAGE_MUST_BE_DISPLAYED y message_TERRAIN_MUST_BE_DISPLAYED
#define message_ELEMENT_MUST_BE_DISPLAYED_s "Debe haber al menos una %s desplegada."
#define message_FIRST_APP_PARAMETER "Archivo PAT a instalar"
#define message_SAVE_CHANGES_ERROR "Error al intentar salvar cambios."

//URL
//#define message_ABSOLUTE_URL_CONVERTION_ERROR "Error al convertir URL a absoluto."
#define message_MISSING_URL "No se encuentra nodo URL."

//Vector

//XML
#define message_XML_LOAD_ERROR "Error al cargar archivo xml."

//Question----------------------------------------------------------------------
#define question_USER_PAT_EXISTS "Ya existe un PAT de usuario con el mismo nombre. Desea reemplazarlo?"
#define question_CONFIRM_PAT_DELETION "Esta seguro que desea eliminar el PAT seleccionado?"
#define question_SAVE_CHANGES "Hay cambios sin guardar. Desea guardarlos?"
#define question_OVERWRITE_DATA "Ya existe un PAT instalado con el mismo nombre. Desea reemplazarlo?"
#define question_FILE_EXISTS "Archivo ya existente. Desea reemplazarlo?"
#define question_EDITED_ELEMENTS "Hay elementos editandose, si continua estos se perderan. Desea continuar?"

//Aplicacion--------------------------------------------------------------------
#define APPLICATION_BUILD "3.0.0" "-" SVN_REVISION

#endif /* MESSAGES_H_ */
