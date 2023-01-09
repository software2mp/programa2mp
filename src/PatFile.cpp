//<COPYRIGHT> a reemplazar a futuro

#include "PatFile.h"

//Includes standard
#include <string>
#include <algorithm>
#include <stdio.h>

//Includes Suri

#include "logmacros.h"
#include "suri/AuxiliaryFunctions.h"
#include "suri/Configuration.h"
#include "suri/xmlnames.h"
#include "suri/Element.h" //para poder obtener el tamanio de los elementos.
#include "suri/FileManagementFunctions.h"
// para que lo incluya en el linkeo
#include "suri/MemoryVectorElement.h"

//Includes Wx
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/filefn.h>
#include <wx/dir.h>
#include "wx/sstream.h"

//Includes App
#include "messages.h"
#include "MainWindow.h"
#include "HotLinkVectorElement.h"

//Defines
#define PATH_PERM 0755
//Defines para nombres de Directorios en el PAT.
#define SPACE " "
#define SEP_DATE "/"

#define SET_ERROR_MSG const_cast<PatFile*>(this)->errorMsg_

#define DOCTYPE_LINE "<!DOCTYPE pat SYSTEM \"pat.dtd\">"

#define HTML_DESCRIPTION "<h1><U>PAT</U>: <I>%s</I>									\
			<img src=\"%s\" align=\"middle\" border=\"0\"></h1>						\
			<ul>																						\
				<li>%s: %s<!--Autor--></li>													\
				<li>%s: %s<!--Fecha--></li>													\
				<li>%s: %s<!--Copyright--></li>												\
			</ul>																						\
			<h2><U>%s<!--Descripcion--></U>:</h2>											\
			<PRE>%s<!--FormatDescription(information, Width)--></PRE>"

#define suritag_DESCRIPTION "<suri src=\"descripcion\"/>"
#define suritag_COPYRIGHT "<suri src=\"copyright\"/>"
#define suritag_DATE "<suri src=\"fecha\"/>"
#define suritag_TITLE "<suri src=\"titulo\"/>"
#define suritag_AUTHOR "<suri src=\"autor\"/>"

/**
 * Objetivo: constructor.
 * \pre FileName debe ser un path absoluto a un archivo, a saber:
 * 	- path absoluto del archivo *.pat
 *		- path absoluto del archivo descripcion.xml
 * @param[in]	FileName: path al arhchivo que identifica un PAT
 */
PatFile::PatFile(const std::string &FileName) :
		filePath_(FileName) {
	errorMsg_.clear();
	wxFileName patname(filePath_);
	fileName_ = wxT("");

	//Verifico la existencia y asigno el tipo de PAT.
	if (!patname.FileExists()) {
		SetErrorMsg(_(message_MISSING_FILE));
		typePat_ = PatInvalid;
		return;
	}

	std::string auxpath = wxT("");
	if (patname.GetExt().MakeLower().Cmp(wxT(extension_PAT)) == 0) //no se traduce, EXTENSION DE ARCHIVO!
			{
		//Creo archivo temporal en la carpeta temp del SO
		std::string pathtempfile;
		pathtempfile = patname.GetTempDir() + patname.GetPathSeparator()
				+ "pat.xml";
		wxFileName fnpathtemp(pathtempfile);
		wxString pathtemp = fnpathtemp.GetVolume().c_str()
				+ (fnpathtemp.HasVolume() ?
						fnpathtemp.GetVolumeSeparator() : wxT(""));
		pathtemp += fnpathtemp.GetFullPath(wxPATH_UNIX).c_str();
		auxpath = patname.CreateTempFileName(pathtemp).c_str();
		if (auxpath.length() == 0) {
			SetErrorMsg(_(message_TEMP_FILE_CREATION_ERROR));
			typePat_ = PatInvalid;
			return;
		}

		suri::ZipFile zip(filePath_);
		/*descomprimo el .xml en un archivo temporal*/
		if (!zip.Extract("descripcion.xml", auxpath, false, true)) {
			SetErrorMsg(_(message_PAT_DESCRIPTION_EXTRACTION_ERROR));
			typePat_ = PatInvalid;
			return;
		}

		if (GetType(auxpath) == PatRelative)
			typePat_ = PatCompressed;
		else {
			SetErrorMsg(
					_(
							wxString::Format("%s %s", message_PAT_CREATE_ERROR,
									message_PAT_INVALID_TYPE).c_str()));
			typePat_ = PatInvalid;
		}
	} else
		typePat_ = GetType();

	//Obtengo el contenido del nodo informacion/nombre
	SetPatName(auxpath);
}

/**
 Objetivo: destructor.
 */
PatFile::~PatFile() {
}

/**
 * Objetivo: Instalar un PAT comprimido.
 * @param[in]	InstallPath: path del directorio donde se debe instalar el PAT.
 * @return PAT instaldo OK? true: false.
 */
bool PatFile::InstallPat(const std::string &InstallPath) const {
	if (typePat_ != PatCompressed) {
		SET_ERROR_MSG.clear();
		SET_ERROR_MSG = wxString::Format(_(message_PAT_FORMAT_ERROR_s),
				filePath_.c_str());
		REPORT_AND_FAIL_VALUE("D:El PAT: %s, no es un PAT comprimido.", false,
				filePath_.c_str());
	}

	//Creo el directorio de instalacion de PATs
	wxFileName file(
			InstallPath + wxFileName::GetPathSeparator() + fileName_
					+ wxFileName::GetPathSeparator(wxPATH_UNIX));
	wxString path = file.GetVolume().c_str()
			+ (file.HasVolume() ? file.GetVolumeSeparator() : wxT(""))
			+ file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
					wxPATH_UNIX);
	if (!file.DirExists()) {
		if (!file.Mkdir(path.c_str(), PATH_PERM, wxPATH_MKDIR_FULL)) {
			SET_ERROR_MSG = wxString::Format(_(message_PAT_DIR_CREATION_ERROR),
					path.c_str());
			REPORT_AND_FAIL_VALUE(
					"D:Error al intentar crear directorio de instalacion de PATs",
					false, path.c_str());
		}
	}

	//se descomprime 1ro el xml para poder verificar la consistencia con el PAT.
	suri::ZipFile zip(filePath_);
	//Extraigo el xml en el directorio path
	if (!zip.Extract("descripcion.xml", path.c_str(), false, true)) {
		SET_ERROR_MSG = _(message_PAT_DESCRIPTION_EXTRACTION_ERROR);
		REPORT_AND_FAIL_VALUE("D:Error al descomprimir archivo descripcion.xml",
				false);
	}

	wxString pathpat = path + "descripcion.xml";
	if (!CheckConsistencyPat(pathpat.c_str(), zip.ListFiles())) {
		SET_ERROR_MSG = _(message_PAT_CONSISTENCY_ERROR);
		REPORT_AND_FAIL_VALUE(
				"D:Inconsistencia entre archivos dentro del PAT y descripcion.xml.",
				false);
	}

	/* TODO(20/02/2009 - Alejandro): Se deberia verificar el formato de los paths
	 * de los elementos y en caso de que no esten utilizando la barra de unix("/")
	 * habria que convertir el xml.
	 * Para eso hay que crear metodos que hagan lo que hoy hace el CreatePatTemp
	 * y modificar los metodos ModifyDependency y ModifyTextureNode.*/

	//Descomprimo el PAT completo.
	if (!zip.Extract(path.c_str(), true, true)) {
		SET_ERROR_MSG = _(message_PAT_INSTALL_ERROR);
		REPORT_AND_FAIL_VALUE("D:Error al instalar PAT", false);
	}

	if (!ModifyHotLinks(pathpat)) {
		SET_ERROR_MSG = _(message_UPDATE_HOTLINKS_ERROR);
		REPORT_AND_FAIL_VALUE("D:Error al modificar hotlinks.", false);
	}

	return true;
}

/**
 * Objetivo: Verificar que el archivo descripcion.xml no tenga referencias a
 * archivos que no se encuentran en el PAT.
 * @param[in]	PathXml: path absoluto del archivo descripcion.xml.
 * @param[in]	FileList: lista de archivos que se deberian hallar en el PAT.
 * @return Ok? true: false.\n
 * \note FileList solo es usada en caso de que el PAT sea comprimido.
 */
bool PatFile::CheckConsistencyPat(const std::string &PathXml,
		const suri::ZipFile::ZipListType &FileList) const {
	std::map < std::string, std::vector<wxXmlNode*> > contextmap;
	std::vector<wxXmlNode*> vecxml;
	contextmap = GetContextNodes(PathXml.empty() ? filePath_ : PathXml);

	std::map<std::string, std::vector<wxXmlNode*> >::iterator it;
	for (it = contextmap.begin(); it != contextmap.end(); it++)
		vecxml.insert(vecxml.end(), it->second.begin(), it->second.end());

	/*agrego el nodo preview para verificar la existencia del archivo.
	 * no corto la ejecucion en caso de no encontrarlo y no lo agrego en caso de
	 * estar vacio.*/
	wxXmlDocument docxml;
	if (!docxml.Load(PathXml, suri::XmlElement::xmlEncoding_)) {
		SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
		REPORT_AND_FAIL_VALUE("D:Error al cargar archivo xml.", false);
	}
	wxXmlNode *pchild = docxml.GetRoot()->GetChildren(); //obtengo nodo info.
	while (pchild && pchild->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //no se traduce, elemento de XML
		pchild = pchild->GetNext();

	if (pchild) {
		wxXmlNode *pchild2 = pchild->GetChildren();
		while (pchild2 && pchild2->GetName().Cmp(wxT(PREVIEW_NODE)) != 0) //no se traduce, elemento de XML
			pchild2 = pchild2->GetNext();
		if (pchild2) {
			//verifico que el nodo url tenga contenido.
			if (!pchild2->GetChildren()->GetNodeContent().IsEmpty())
				vecxml.push_back(pchild2);
		}
	}

	return CheckConsistencyPat(vecxml, FileList);
}

/**
 * Objetivo: Verificar que el archivo descripcion.xml no tenga referencias a
 * archivos que no se encuentran en el PAT.
 * \pre Para poder verificar la imagen de vista previa del PAT, se debe
 * agregar al vector de nodos elemento el nodo PREVIEW del nodo informacion, en
 * el metodo que llama a CheckConsistencyPat(VecNodes, FileList).
 * (ej: CheckConsistencyPat(PathXml, FileList))
 * @param[in]	VecNodes: Vector de nodos elemento
 * @param[in]	FileList: lista de archivos que se deberian hallar en el PAT.
 * @return Ok? true: false.\n
 * \note FileList solo es usada en caso de que el PAT sea comprimido.\n
 */
bool PatFile::CheckConsistencyPat(std::vector<wxXmlNode*> VecNodes,
		const suri::ZipFile::ZipListType &FileList) const {
	size_t vecsize = VecNodes.size();
	size_t lstsize = FileList.size();

	if (GetPatType() == PatCompressed) {
		if (vecsize == 0) {
			SET_ERROR_MSG = _(message_PAT_ELEMENTS_NOT_FOUND);
			return false;
		}

		if (lstsize == 0) {
			SET_ERROR_MSG = _(message_NO_FILES_IN_PAT);
			return false;
		}

		//verifico que todos los elementos del xml se encuentren en la lista.
		for (size_t i = 0; i < vecsize; i++) {
			wxXmlNode *pnode = VecNodes.at(i)->GetChildren();
			while (pnode && pnode->GetName().Cmp(wxT(URL_NODE)) != 0) //no se traduce, elemento de XML
				pnode = pnode->GetNext();
			wxFileName fnfile(pnode->GetNodeContent().c_str());

			wxString pathfile = fnfile.GetVolume().c_str()
					+ (fnfile.HasVolume() ?
							fnfile.GetVolumeSeparator() : wxT(""))
					+ fnfile.GetFullPath(wxPATH_UNIX).c_str();

			if (!FindFileList(pathfile.c_str(), FileList)) {
				SET_ERROR_MSG = wxString::Format(_(message_PAT_MISSING_FILE_s),
						pnode->GetNodeContent().c_str());
				return false;
			}
		}

		//verifico las dependencias de archivos.
		if (!CheckDependency(GetDependency(VecNodes), FileList)) {
			SET_ERROR_MSG = _(message_MISSING_PAT_DEPENDENCY);
#ifdef __DEBUG__
			REPORT_DEBUG("D: Dependencias que fallaron:");
			suri::ZipFile::ZipListType::const_iterator it = FileList.begin();
			while (it != FileList.end()) {
				REPORT_DEBUG("D: * %s", (*it).c_str());
				it++;
			}
#endif
			return false;
		}
		return true;
	}

	/*Si el PAT no es Comprimido hay que verificar la existencia de los
	 * archivos descritos en el xml.*/
	for (size_t i = 0; i < vecsize; i++) {
		std::string pathaux = wxT("");
		if (GetPatType() == PatRelative) //tengo que formar el path absoluto del archivo.
				{
			wxFileName fnxml(filePath_);
			pathaux = fnxml.GetVolume().c_str()
					+ (fnxml.HasVolume() ? fnxml.GetVolumeSeparator() : wxT(""))
					+ fnxml.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
							wxPATH_UNIX);
		}

		wxXmlNode *pnode = VecNodes.at(i)->GetChildren();
		while (pnode && pnode->GetName().Cmp(URL_NODE) != 0)
			pnode = pnode->GetNext();

		if (!pnode) {
			SET_ERROR_MSG = _(message_MISSING_URL);
			return false;
		}
		wxFileName fnfile(pathaux + pnode->GetNodeContent().c_str());
		if (!fnfile.FileExists() && !wxDir::Exists(fnfile.GetFullPath())) {
			SET_ERROR_MSG = wxString::Format(_(message_FILE_NOT_FOUND_s),
					fnfile.GetFullName().c_str());
			return false;
		}
		//verifico las dependencias de archivos.
		if (!CheckDependency(GetDependency(VecNodes))) {
			SET_ERROR_MSG = _(message_MISSING_PAT_DEPENDENCY);
			return false;
		}
	}
	return true;
}

/**
 * Objetivo: Crea un PAT comprimido con los archivos en el directorio pasado en
 * el Constructor, con el nombre PatFileName.
 * @param[in]	PatFileName nombre del archivo PAT a crear.
 * @return	true si pudo crear el PAT comprimido.
 * @return	false en caso de error.
 */
bool PatFile::CreatePatFile(const std::string &PatFileName) const {
	if (GetPatType() != PatRelative && GetPatType() != PatAbsolute) {
		SET_ERROR_MSG = _(
				wxString::Format("%s %s", message_PAT_CREATE_ERROR,
						message_PAT_INVALID_TYPE).c_str());
		return false;
	}

	//comento ya que la validacion se pasa a PatManagerWidget::OnButtonCompress.
#ifdef __UNUSED_CODE__
	// verifico que haya espacio suficiente para poder crear el PAT.
	if(!CheckPatSize(wxFileName(PatFileName).GetPathWithSep().c_str()))
	{
		SET_ERROR_MSG = _(message_PAT_NOT_ENOUGH_SPACE);
		return false;
	}
#endif

	//Verifico que existan los archivos en el FS
	if (!CheckConsistencyPat(filePath_))
		return false;

	wxFileName fnpathxml(filePath_);
	wxString pathtempxml =
			fnpathxml.GetVolume().c_str()
					+ (fnpathxml.HasVolume() ?
							fnpathxml.GetVolumeSeparator() : wxT(""));
	pathtempxml += fnpathxml.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
			wxPATH_UNIX).c_str();
	pathtempxml += fnpathxml.GetFullName().c_str();

	suri::ZipFile::ZipListType filelist, pathlist;
	filelist.clear();
	pathlist.clear();
	//Genero un xml temporal, con referencias relativas a archivos.
	if (!CreatePatTemp(pathtempxml.c_str(), filelist, pathlist)) {
		//elimino el archivo temporal.
		if (!wxRemoveFile(pathtempxml.c_str()))
			REPORT_DEBUG(
					"D:Error al intentar eliminar archivo: %s. Verifique que exista y que no este siendo usado.",
					pathtempxml.c_str());
		SET_ERROR_MSG = _(message_PAT_CREATE_ERROR);
		return false;
	}

	if (filelist.empty()) {
		SET_ERROR_MSG = _(message_EMPTY_PAT_ERROR);
		return false;
	}

	filelist.push_front(pathtempxml.c_str());
	pathlist.push_front("descripcion.xml");

	suri::ZipFile zip(PatFileName);
	if (!zip.Add(filelist, pathlist, true)) {
		//elimino el archivo temporal.
		if (!wxRemoveFile(pathtempxml.c_str()))
			REPORT_DEBUG(
					"D:Error al intentar eliminar archivo: %s. Verifique que exista y que no este siendo usado.",
					pathtempxml.c_str());
		SET_ERROR_MSG = _(message_PAT_FILE_ADD_ERROR);
		return false;
	}

	/* vuelvo a poner un path absoluto al hotlink en el .dbf*/
	wxString pathxmlorg =
			fnpathxml.GetVolume().c_str()
					+ (fnpathxml.HasVolume() ?
							fnpathxml.GetVolumeSeparator() : wxT(""));
	pathxmlorg += fnpathxml.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
			wxPATH_UNIX).c_str();
	pathxmlorg += wxT("descripcion.xml");
	//pathxmlorg += fnpathxml.GetExt().c_str();
	ModifyHotLinks(pathxmlorg);

	//Borro el xml temporal.
	if (!wxRemoveFile(pathtempxml.c_str()))
		REPORT_DEBUG(
				"D:Error al intentar eliminar archivo: %s. Verifique que exista y que no este siendo usado.",
				pathtempxml.c_str());
	return true;
}

/** Retorna el arbol */
wxXmlNode* PatFile::GetContexts(wxXmlDocument* docxml) {
	if (!docxml->Load(filePath_, suri::XmlElement::xmlEncoding_)) {
		SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
	}
	wxXmlNode *pcontexts = docxml->GetRoot()->GetChildren();

	return pcontexts;
}

/**
 * Objetivo: Obtiene todos los nodos "elemento" del archivo descripcion.xml.
 * @param[in]	PathXml: path absoluto del archivo descripcion.xml.
 * @return	vector<wxXmlNode*>: devuelve un vector de ptr. a nodos del xml.\n
 * NOTA: los ptr. del vector no se pierden al destruir el objeto PatFile.
 */
std::map<std::string, std::vector<wxXmlNode*> > PatFile::GetContextNodes(
		const std::string& PathXml) const {
	std::map < std::string, std::vector<wxXmlNode*> > contextmap;
	std::vector<wxXmlNode*> vecxml;
	std::string contextname = "";
	wxXmlDocument docxml;
   std::string encoding = suri::XmlElement::xmlEncoding_;
#ifdef __LINUX__
   encoding = suri::XmlElement::utf8XmlEncoding_;
#endif
   if (!docxml.Load(PathXml.empty() ? filePath_ : PathXml, encoding)) {
		vecxml.clear();
		SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
		REPORT_AND_FAIL_VALUE("D:Error al cargar archivo xml.", contextmap);
	}

	wxXmlNode *pchild = docxml.GetRoot()->GetChildren();
	wxXmlNode *pchild2;
	while (pchild) //Recorro los nodos, buscando los elementos.
	{
		if (pchild->GetName().Cmp(wxT(CONTEXT_NODE)) == 0) //no se traduce, elemento de XML
				{
			if ((contextname = pchild->GetPropVal(NAME_PROPERTY, "")).empty())
				REPORT_AND_FAIL_VALUE("D:Contexto sin nombre", contextmap);

			pchild2 = pchild->GetChildren();
			while (pchild2)
				if (pchild2->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)) == 0
						|| pchild2->GetName().Cmp(wxT(GROUP_NODE)) == 0) //no se traduce, elemento de XML
						{
					wxXmlNode *pchildaux = pchild2;
					vecxml.push_back(pchildaux);
					pchild2 = pchild2->GetNext();
					if (!pchild->RemoveChild(pchildaux)) {
						vecxml.clear();
						REPORT_AND_FAIL_VALUE(
								"D:No se pudo eliminar el Nodo: %s",
								contextmap,
								pchildaux->GetChildren()-> GetNext()->GetNodeContent().c_str());
					}
				} else
					pchild2 = pchild2->GetNext();
		}

		if (vecxml.size() > 0) {
			if (!(contextmap.insert(std::make_pair(contextname, vecxml))).second)
				REPORT_DEBUG("D:Contexto ya ingresado.");
			contextname.clear();
			vecxml.clear();
		}

		pchild = pchild->GetNext();
	}
	delete pchild;
	return contextmap;
}

/**
 * Objetivo: devuelve el nombre del PAT
 * @return string con el nombre del PAT o la cadena vacia en caso de ser un PAT
 * invalido.
 */
std::string PatFile::GetPatName() const {
	return fileName_;
}

//Funcion no utilizada
#ifdef __UNUSED_CODE__
/**
 * @return Devuelve un html con el contenido del nodo descripcion del xml.
 */
/*
 std::string PatFile::GetDescription(const int &Width)
 {
 std::string pathxml;
 std::string pathimg;
 std::string name;

 //devuelve los paths ya normalizados.
 GetPaths(filePath_, pathxml, pathimg);

 wxXmlDocument docxml;
 wxFileName fnxml(pathxml);
 wxString pathxmlaux = fnxml.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR,
 wxPATH_UNIX);
 pathxmlaux += fnxml.GetFullName().c_str();
 if(!docxml.Load(pathxmlaux.c_str(), suri::XmlElement::xmlEncoding_))
 {
 SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
 return message_PAT_DESCRIPTION_ERROR;
 }

 //Obtengo nodo informacion.
 wxXmlNode *pchild = docxml.GetRoot()->GetChildren();
 while(pchild && pchild->GetName().Cmp(wxT(INFORMATION_NODE))!=0) //no se traduce, elemento de XML
 pchild = pchild->GetNext();

 if(!pchild)
 return message_PAT_DESCRIPTION_ERROR;

 //obtengo datos de los nodos que componen el nodo informacion.
 wxXmlNode *pchild2 = pchild->GetChildren();
 std::string author;
 std::string date;
 std::string copyright;
 std::string information;
 while(pchild2) //recorro nodos del nodo informacion.
 {
 if(pchild2->GetName().Cmp(wxT(NAME_NODE))==0) //no se traduce, elemento de XML
 {
 name = ParseNode(pchild2->GetChildren(), SPACE);
 pchild2 = pchild2->GetNext();
 continue;
 }
 if(pchild2->GetName().Cmp(wxT(AUTHOR_NODE))==0) //no se traduce, elemento de XML
 {
 author = ParseNode(pchild2->GetChildren(), SPACE);
 pchild2 = pchild2->GetNext();
 continue;
 }
 if(pchild2->GetName().Cmp(wxT(DATE_NODE))==0) //no se traduce, elemento de XML
 {
 date = _(content_INVALID_DATE);
 if(VerifyDateNode(pchild2))
 date = ParseNode(pchild2->GetChildren(), SEP_DATE);
 pchild2 = pchild2->GetNext();
 continue;
 }
 if(pchild2->GetName().Cmp(wxT(COPYRIGHT_NODE))==0) //no se traduce, elemento de XML
 {
 copyright = ParseNode(pchild2->GetChildren());
 pchild2 = pchild2->GetNext();
 continue;
 }
 if(pchild2->GetName().Cmp(wxT(DESCRIPTION_NODE))==0) //no se traduce, elemento de XML
 {
 information = ParseNode(pchild2->GetChildren());
 pchild2 = pchild2->GetNext();
 continue;
 }
 pchild2 = pchild2->GetNext();
 }

 char htmldesc[2048];
 snprintf(htmldesc, 2048, HTML_DESCRIPTION, name.c_str(), pathimg.c_str(),
 wxT("Autor"),author.empty()?_(content_FALLBACK_AUTHOR):author.c_str(), //no se traduce
 wxT("Fecha"),date.empty()?_(content_FALLBACK_DATE):date.c_str(), //no se traduce
 wxT("Copyright"),copyright.c_str(), //no se traduce
 wxT("Descripcion"),FormatDescription(information, Width).c_str()); //no se traduce
 return std::string(htmldesc);
 }
 */
#endif

/**
 * Devuelve la Descripcion del PAT.
 * Devuelve un mapa con los valores del nodo descripcion y una clave asociada.
 * @param[out]	Params: mapa con los valores del nodo descripcion.
 * @return true obtencion exitosa de los datos
 * @return false en caso de que no se puedan obtener los datos
 */
bool PatFile::GetPatDescription(std::map<std::string, std::string> &Params) {
	std::string pathxml;
	std::string pathimg;
	std::string name;

	//devuelve los paths ya normalizados.
	GetPaths(filePath_, pathxml, pathimg);
	wxXmlDocument docxml;
   std::string encoding = suri::XmlElement::xmlEncoding_;
   if (!docxml.Load(pathxml.c_str(), encoding)) {
		SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
		return false;
	}

	//Obtengo nodo informacion.
	wxXmlNode *pchild = docxml.GetRoot()->GetChildren();
	while (pchild && pchild->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //no se traduce, elemento de XML
		pchild = pchild->GetNext();

	if (!pchild)
		return false;

	//obtengo datos de los nodos que componen el nodo informacion.
	wxXmlNode *pchild2 = pchild->GetChildren();
	while (pchild2) //recorro nodos del nodo informacion.
	{
		if (pchild2->GetName().Cmp(wxT(NAME_NODE)) == 0) //no se traduce, elemento de XML
				{
			wxString title = ParseNode(pchild2->GetChildren(), SPACE);
			Params.insert(std::make_pair(suritag_TITLE, title.c_str()));
			pchild2 = pchild2->GetNext();
			continue;
		}
		if (pchild2->GetName().Cmp(wxT(AUTHOR_NODE)) == 0) //no se traduce, elemento de XML
				{
			wxString author = ParseNode(pchild2->GetChildren(), SPACE);
			Params.insert(std::make_pair(suritag_AUTHOR, author));
			pchild2 = pchild2->GetNext();
			continue;
		}
		if (pchild2->GetName().Cmp(wxT(DATE_NODE)) == 0) //no se traduce, elemento de XML
				{
			//date = _("Fecha invalida");
			if (VerifyDateNode(pchild2))
				Params.insert(
						std::make_pair(suritag_DATE,
								ParseNode(pchild2->GetChildren(), SEP_DATE)));
			pchild2 = pchild2->GetNext();
			continue;
		}
		if (pchild2->GetName().Cmp(wxT(COPYRIGHT_NODE)) == 0) //no se traduce, elemento de XML
				{
			wxString copyr = ParseNode(pchild2->GetChildren());
			Params.insert(std::make_pair(suritag_COPYRIGHT, copyr.c_str()));
			pchild2 = pchild2->GetNext();
			continue;
		}
		if (pchild2->GetName().Cmp(wxT(DESCRIPTION_NODE)) == 0) //no se traduce, elemento de XML
				{
			wxString desc = ParseNode(pchild2->GetChildren());
			Params.insert(std::make_pair(suritag_DESCRIPTION, desc));
			pchild2 = pchild2->GetNext();
			continue;
		}
		pchild2 = pchild2->GetNext();
	}

	if (!pathimg.empty()) {
		std::string tag = "<img src=\"";
		tag += pathimg.c_str();
		tag += "\"";
		wxImage img(pathimg);
		if (img.IsOk()) {
			if (img.GetWidth() > 230)
				tag += " width=\"230\"";
			if (img.GetHeight() > 230)
				tag += " height=\"230\"";
		}
		tag += " alt=\"Prevista PAT\"/>";
		Params.insert(
				std::make_pair(
						wxT(
								"<suri src=\"preview\" max-width=\"230\" alt=\"Prevista PAT\"/>"),
						tag));
	}
	return true;
}

/**
 * Objetivo: devuelve una copia del nodo informacion.
 * @param[in]	PathXml: path del archivo descripcion.xml.
 * @return copia del nodo informacion o null en caso de error.
 * \attention quien invoque a este metodo es responsable del puntero devuelto,
 * por lo tanto debe hacerse cargo de su eliminacion.
 */
wxXmlNode * PatFile::GetInfoNode(const std::string &PathXml)

{
	wxXmlDocument docxml;
	wxFileName fnxml(PathXml.empty() ? filePath_ : PathXml);

	wxString pathxml = fnxml.GetVolume().c_str()
			+ (fnxml.HasVolume() ? fnxml.GetVolumeSeparator() : wxT(""))
			+ fnxml.GetFullPath(wxPATH_UNIX);
	if (!docxml.Load(pathxml, suri::XmlElement::xmlEncoding_))
		REPORT_AND_FAIL_VALUE("D:Error al cargar archivo xml.", NULL);

	wxXmlNode *pnode = docxml.GetRoot()->GetChildren();
	while (pnode && pnode->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //no se traduce, elemento de XML
		pnode = pnode->GetNext();
	if (pnode)
		return new wxXmlNode(*pnode);

	return NULL;
}

//! Devuelve una copia del nodo raiz
/**
 * Carga y retorna una copia del Xml que representa al PAT.
 * @param XmlPath Ruta del archivo XML o vacio para el interno
 * @return copia del nodo Xml representando al PAT
 * @attention El valor retornado debe ser liberado por el codigo cliente
 */
wxXmlNode *PatFile::GetRootNode(const std::string &XmlPath) {
	wxXmlDocument docxml;
	wxFileName fnxml(XmlPath.empty() ? filePath_ : XmlPath);

	wxString pathxml = fnxml.GetVolume().c_str()
			+ (fnxml.HasVolume() ? fnxml.GetVolumeSeparator() : wxT(""))
			+ fnxml.GetFullPath(wxPATH_UNIX);
	if (!docxml.Load(pathxml, suri::XmlElement::xmlEncoding_))
		REPORT_AND_FAIL_VALUE("D:Error al cargar archivo xml.", NULL);

	wxXmlNode *pnode = docxml.GetRoot();

	if (pnode)
		return new wxXmlNode(*pnode);

	return NULL;
}

/**
 * Devuelve el nodo Dependencias del elemento.
 * \post El puntero retornado pertenece a la jerarquia del parametro pElementNode, no
 *       debera ser eliminado.
 * @param[in]	pElementNode: nodo elemento.
 * @return nodo dependencias del elemento o NULL en caso de error.
 * \attention el puntero devuelto no debe ser eliminado.
 */
wxXmlNode * PatFile::GetDependencyNode(const wxXmlNode *pElementNode) const {
	if (!pElementNode)
		return NULL;
	//busco el nodo dependencias.
	wxXmlNode *pnode = pElementNode->GetChildren();
	while (pnode && pnode->GetName().Cmp(wxT(DEPENDENCY_NODE)) != 0) //no se traduce, elemento de XML
		pnode = pnode->GetNext();

	return pnode;
}

/**
 * Salva un string que representa un XML en un archivo
 * \pre El string XmlString debe reprentar un xml valido.
 * \pre XmlString puede o no tener DOCTYPE
 * \post Se agrega el DOCTYPE para PATs si no existiese
 * @param[in]	FileName: path del archivo xml a salvar.
 * @param[in]	XmlString: contenido del xml.
 * @return true en caso de poder salvar el xml. false C.O.C.
 */
bool PatFile::SaveXml(const wxString &FileName, const wxString &XmlString) {
	wxFileName fn(FileName);
	if (!fn.Mkdir(fn.GetPath(), 0755, wxPATH_MKDIR_FULL))
		REPORT_AND_FAIL_VALUE("D:Error al crear directorio %s.", false,
				fn.GetPath().c_str());

	wxString xmlstring = XmlString;
	//Agrego linea de DocType
	if (!AddDocType(xmlstring))
		REPORT_AND_FAIL_VALUE("D:No se pudo agregar linea de DocType.", false);

	//Grabo contenido del xml
	wxFFile ffile(FileName, "w");
	if (!ffile.IsOpened())
		REPORT_AND_FAIL_VALUE("D:Error al intentar grabar archivo: %s.", false,
				FileName.c_str());
	if (!ffile.Write(xmlstring))
		REPORT_AND_FAIL_VALUE("D:Error al escribir archivo: %s.", false,
				FileName.c_str());
	ffile.Flush();
	if (!ffile.Close())
		REPORT_AND_FAIL_VALUE("D:Error al cerrar archivo : %s.", false,
				FileName.c_str());
	return true;
}

/**
 * Salva un Nodo Xml a un string. Guarda en el string el XML que prepresenta al
 * nodo pasado por parametro.
 * @param[in] pRootNode Nodo raiz. No toma pocesion.
 * @param[out] XmlString String con el XML
 */
bool PatFile::SaveXml(wxString &XmlString, wxXmlNode *pRootNode) {
	XmlString.clear();
	wxXmlDocument doc;
	doc.SetRoot(pRootNode);
	doc.SetEncoding(suri::XmlElement::xmlEncoding_);
	doc.SetFileEncoding(suri::XmlElement::xmlEncoding_);
	wxStringOutputStream ss(&XmlString);
	doc.Save(ss);
	doc.DetachRoot();
	return AddDocType(XmlString);
}

//! agrega la linea doctype a un string que representa un XML
/**
 * Objetivo: Agrega linea de DocType a un XML
 * \todo Si existe el doctype hace el reemplazo cuando se podria dejarlo, esto
 *      queda ya que puede servir para reemplzar el doctype por otro en lugar de
 *      si mismo como es ahora.
 * @param[in]	XmlString el string con el xml
 * @return	Agrega Ok? true: false;
 */
bool PatFile::AddDocType(wxString &XmlString) {
	// si ya trae el doctype, lo reemplazo (asumo que viene bien puesto)
	int start = XmlString.Find(DOCTYPE_LINE);
	// si no encontro, no reemplaza, inserta
	int len = start == wxNOT_FOUND ? 0 : wxString(DOCTYPE_LINE).length() + 1;
	// si no lo tiene, el punto de insercion es la segunda linea
	start = start == wxNOT_FOUND ? XmlString.Find('\n') : start - 1;
	wxString predoctype = XmlString.Mid(0, start + 1);
	wxString postdoctype = XmlString.Mid(start + len);
	XmlString = predoctype + DOCTYPE_LINE + postdoctype;
	return true;
}

/**
 * Verifica si hay espacio suficiente en disco para instalar/crear PAT. El
 * criterio utilizado para determinar si hay o no espacio es el siguiente:
 * 	- Para poder crear PAT (comprimido) debe haber espacio suficiente para
 * poder realizar una copia del PAT(sin comprimir) al momento de comprimir.
 * 	- Para poder instalar PAT debe haber espacio suficiente para poder
 * descomprimir todos los archivos dentro del PAT.
 * @param[in]	DestinyPath path absoluto del path de destino (donde se va a
 * crear/instalar el PAT).
 * @param[in]	PathXml path absoluto del archivo descripcion.xml.
 * @return true en caso de que haya espacio suficiente
 * @return false en caso de que no haya espacio suficiente.
 * \note no se puede confiar 100% en el tamanio devuelto por la funcion
 * GetFileSize ya que si el PAT a verificar contiene HotLinks, no se tiene en
 * cuenta el tamanio del html ni de la imagen(del HL) ya que estos no se
 * incluyen en el nodo dependencias del elemento.
 */
bool PatFile::CheckPatSize(const std::string &DestinyPath,
		const std::string &PathXml) const {
	// obtengo vector de nodos elemento.
	std::vector<wxXmlNode *> vecelementsnode;
	GetElementsNode(vecelementsnode, PathXml, typePat_ == PatRelative);
	// obtengo el tamanio de los elementos.
	wxULongLong size = GetFilesSize(vecelementsnode);
	if (size == wxInvalidSize)
		return false;
	wxLongLong sizeaux(size.GetHi(), size.GetLo()); //creo para poder comparar.
	// obtengo el tamanio libre en disco.
	wxLongLong freespace = 0;
	wxGetDiskSpace(DestinyPath, NULL, &freespace);
	if (freespace < sizeaux)
		SET_ERROR_MSG = _(message_PAT_NOT_ENOUGH_SPACE);
	return freespace > sizeaux;
}

// ------------------------- PRIVADAS ----------------------------
/**
 * Objetivo: Devuelve el tipo de PAT con el que se va a trabajar.
 * @param[in]	PathXml: path absoluto del archivo descripcion.xml.
 * @return	Ok? PatRelative | PatAbsolute : PatInvalid.
 */
PatFile::PatType PatFile::GetType(const std::string &PathXml) const {
	wxXmlDocument docxml;
	wxFileName *pfnxml;
	if (PathXml.empty())
		pfnxml = new wxFileName(filePath_);
	else
		pfnxml = new wxFileName(PathXml);

	wxString pathxml = pfnxml->GetVolume().c_str()
			+ (pfnxml->HasVolume() ? pfnxml->GetVolumeSeparator() : wxT(""))
			+ pfnxml->GetFullPath(wxPATH_UNIX);
	delete pfnxml;
	if (!docxml.Load(pathxml, suri::XmlElement::xmlEncoding_))
		REPORT_AND_FAIL_VALUE("D:Error al cargar archivo xml.", PatInvalid);

	wxXmlNode *pnode = docxml.GetRoot();
	bool found = false;
	while (pnode && !found) {
		if (pnode->GetName().Cmp(wxT(PAT_NODE)) == 0) //no se traduce, elemento de XML
				{
			wxXmlProperty *pxmlprop = pnode->GetProperties();
			while (pxmlprop) {
				if (pxmlprop->GetName().Cmp(wxT(TYPE_NODE)) == 0) //no se traduce, elemento de XML
						{
					if (pxmlprop->GetValue().Cmp(
							wxT(TYPE_PROPERTY_VALUE_PAT_RELATIVE)) == 0) //no se traduce, elemento de XML
						return PatRelative;
					if (pxmlprop->GetValue().Cmp(
							wxT(TYPE_PROPERTY_VALUE_PAT_ABSOLUTE)) == 0) //no se traduce, elemento de XML
						return PatAbsolute;
				}
				pxmlprop = pxmlprop->GetNext();
			}
			found = true;
		}

		if (found)
			return PatInvalid;

		pnode = pnode->GetNext();
	}
	return PatInvalid;
}

/**
 * Objetivo: Genera un archivo xml temporal para guardarlo en un PAT comprimido.
 * @param[out]	PathTempXml: path absoluto del xml temporal.
 * @param[out]	FileList: lista con los path absolutos de los archivos a agregar al zip.
 * @param[out]	DirList: lista con los directorios relativos, de los archivos.
 * @return	xml temporal ok? true:false.
 */
bool PatFile::CreatePatTemp(const std::string &PathTempXml,
		suri::ZipFile::ZipListType &FileList,
		suri::ZipFile::ZipListType &DirList) const {
	//Cargo el xml
	wxXmlDocument docxml;
	if (!docxml.Load(filePath_, suri::XmlElement::xmlEncoding_)) {
		SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
		REPORT_AND_FAIL_VALUE("D:Error al cargar archivo xml.", false);
	}

	wxXmlNode *pnode = docxml.GetRoot();
	if (!pnode)
		REPORT_AND_FAIL_VALUE("D:Error al obtener nodo raiz", false);

	if (GetPatType() == PatAbsolute) {
		/*Cambio el nodo raiz del xml:
		 *<pat tipo="absoluto" version="1.0"> x <pat tipo="relativo" version="1.0">*/
		wxXmlProperty *pprop = pnode ? pnode->GetProperties() : NULL;
		//Recorro las propiedades del nodo raiz
		while (pprop && (pprop->GetName().Cmp(wxT(TYPE_NODE)) != 0)) //no se traduce, elemento de XML
			pprop = pprop->GetNext();
		if (!pprop)
			REPORT_AND_FAIL_VALUE("D:Sin propiedad <tipo>.", false);
		pprop->SetValue(wxT(TYPE_PROPERTY_VALUE_PAT_RELATIVE)); //no se traduce, elemento de XML
	}

	//Recorro los nodos, buscando los elementos.
	wxXmlNode *pchild = pnode->GetChildren();
	int nroelem;
	while (pchild) {
		//Agrego la imagen del preview.
		if (pchild->GetName().Cmp(wxT(INFORMATION_NODE)) == 0) //no se traduce, elemento de XML
			AddPreviewImg(pchild, FileList, DirList);

		if (pchild->GetName().Cmp(wxT(CONTEXT_NODE)) == 0) //no se traduce, elemento de XML
				{
			nroelem = 0; //es para armar el path del elemento. Ej: Elem_0, Elem_1
			//obtengo el nodo elemento
			wxXmlNode *pelementnode = pchild->GetChildren();
			while (pelementnode) {
				if (pelementnode->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)) == 0) //no se traduce, elemento de XML
						{
					wxXmlNode *purlnode = pelementnode->GetChildren();
					while (purlnode
							&& (purlnode->GetName().Cmp(wxT(URL_NODE)) != 0)) //no se traduce, elemento de XML
						purlnode = purlnode->GetNext();
					if (purlnode) {
						wxFileName fnurl(purlnode->GetNodeContent());
						/* Obtengo el nodo hijo del nodo url, que es un wxXML_TEXT_NODE
						 * para poder modificar su contenido*/
						wxXmlNode *pnodeaux = purlnode->GetChildren();
						std::string newpathnode = wxT(""); //nuevo path del nodo url
						std::string pathurl = wxT("");
						std::string pathrelative = wxT(""); //path dentro del PAT
						wxString pathfile = wxT(""); //path (absoluto) del archivo a guardar en el PAT.
						if (!pnodeaux)
							continue;
						if (GetPatType() == PatAbsolute) {
							//genero el path del archivo a guardar en el zip
							pathfile.Empty();
							pathfile =
									fnurl.GetVolume().c_str()
											+ (fnurl.HasVolume() ?
													fnurl.GetVolumeSeparator() :
													wxT(""));
							pathfile += fnurl.GetPath(
									wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
									wxPATH_UNIX);
							pathfile += fnurl.GetFullName().c_str();
							//genero el path que va a tener el archivo dentro del zip
							pathrelative.clear();
							pathrelative = GetPathElement(pelementnode,
									nroelem++, GetContextName(pchild),
									wxDir::Exists(pathfile));
							if (!pathrelative.empty()) {
								//genero el nuevo cotenido del nodo url
								newpathnode.clear();
								newpathnode = pathrelative
										+ fnurl.GetPathSeparator(wxPATH_UNIX)
										+ fnurl.GetFullName().c_str();
							}
							pathurl =
									fnurl.GetVolume().c_str()
											+ (fnurl.HasVolume() ?
													fnurl.GetVolumeSeparator() :
													wxT(""));
							pathurl += fnurl.GetPath(
									wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
									wxPATH_UNIX);
						} else //PatRelative
						{
							wxFileName fn(filePath_);
							//genero el path del archivo a guardar en el zip
							pathfile.Empty();
							pathfile = fn.GetVolume().c_str()
									+ (fn.HasVolume() ?
											fn.GetVolumeSeparator() : wxT(""));
							pathfile += fn.GetPath(wxPATH_GET_VOLUME,
									wxPATH_UNIX);
							pathfile += fn.GetPathSeparator(wxPATH_UNIX);
							pathfile += fnurl.GetFullPath(wxPATH_UNIX);
							//genero el path que va a tener el archivo dentro del zip
							pathrelative.clear();
							pathrelative = fnurl.GetPath(wxPATH_GET_VOLUME,
									wxPATH_UNIX);
							//genero el nuevo cotenido del nodo url
							newpathnode.clear();
							newpathnode = fnurl.GetFullPath(wxPATH_UNIX);

							pathurl.clear();
							pathurl = fn.GetVolume().c_str()
									+ (fn.HasVolume() ?
											fn.GetVolumeSeparator() : wxT(""));
							pathurl += fn.GetPath(
									wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
									wxPATH_UNIX)
									+ fnurl.GetPath(
											wxPATH_GET_VOLUME
													| wxPATH_GET_SEPARATOR,
											wxPATH_UNIX);
						}

						//agrego el arch. a la lista de arch. que se guardan en el zip
						FileList.push_back(pathfile.c_str());
						//agrego el directorio que va a tener el arch. dentro del zip.
						DirList.push_back(pathrelative);
						//Modifica los path de los nodos url del nodo dependencia
						ModifyDependency(pelementnode, pathrelative, pathurl,
								FileList, DirList);
						//Cambia el nodo textura.
						ModifyTextureNode(pelementnode, pathrelative, pathurl);

						/* Agrego los Hotlinks
						 * Los hotlinks, no se agregan en el nodo dependencias ya que
						 * las dependencias se agregan relativas al elemento que se
						 * este procesando, por lo tanto, no se generaria el
						 * directorio: Pat - xxx/comun/hotlinks/...
						 * Para corregirlo se procesa el nodo "caracteristicas", que
						 * se genera solo en el caso de tener un vector de puntos que
						 * ademas tenga el nodo "hotlink" dentro de cada nodo "capa"
						 * dentro del nodo "capas".
						 * Esto al igual que el agregado de las dependencias,
						 * modificacion de los nodos de terreno, deberia modificarse
						 * y ser responsabilidad de cada elemento.
						 */
						AddHotlinks(pelementnode, FileList, DirList);

						//seteo el nuevo valor del nodo url del elemento.
						pnodeaux->SetContent(newpathnode);
					}
				}
				pelementnode = pelementnode->GetNext();
			}
		}
		pchild = pchild->GetNext();
	}

	//Grabo el xml con distinto nombre para no pisar el original.
	wxString xmlstring;
	if (!PatFile::SaveXml(xmlstring, docxml.GetRoot())
			|| !PatFile::SaveXml(PathTempXml, xmlstring))
		REPORT_AND_FAIL_VALUE("D:Error al escribir archivo: %s.", false,
				PathTempXml.c_str());

	return true;
}

/**
 * Objetivo: Verificar si PathFile se encuentra en la lista FileList.
 * @param[in]	PathFile: path de un archivo(abs. o rel).
 * @param[in]	FileList: Lista de path de archivos.
 * @return existe? true: false;
 */
bool PatFile::FindFileList(const std::string &PathFile,
		const suri::ZipFile::ZipListType &FileList) const {
	bool found = false;
	wxFileName fnvec(PathFile);
   wxString patfile = PathFile;
   patfile = patfile.MakeUpper();
	/* esto es por el caso de los vectores, ya que el contenido del nodo url es
	 * un directorio y por lo tanto no viene detallado en la lista de archivos a
	 * verificar */
	if (fnvec.IsDir() || !fnvec.HasExt())
		return true;

	for (size_t i = 0; i < FileList.size() && !found; i++) {
		wxString file = FileList.at(i);
		if (file.MakeUpper().Cmp(wxT("DESCRIPCION.XML")) != 0) //El xml no se traduce
			if (file.MakeUpper().Cmp(patfile) == 0)
				found = true;
	}
	return found;
}

/**
 * Objetivo:	Obtiene la propiedad "nombre" de un nodo contexto
 * @param[in]	pContextNode: nodo contexto
 * @return Ok? propiedad "nombre":""
 * Precondicion: pContextNode debe ser un puntero a un nodo contexto no nulo.
 */
std::string PatFile::GetContextName(const wxXmlNode *pContextNode) const {
	if (!pContextNode)
		return wxT("");

	//Obtengo el Nombre de Contexto
	wxXmlProperty *ppropcontext = pContextNode->GetProperties();
	while (ppropcontext && (ppropcontext->GetName().Cmp(wxT(NAME_NODE)) != 0)) //El xml no se traduce
		ppropcontext = ppropcontext->GetNext();

	if (!ppropcontext) {
		REPORT_DEBUG("D:No existe propiedad <nombre> en nodo <contexto>");
		return "";
	}

	return ppropcontext->GetValue().c_str();
}

/**
 * Objetivo: Devuelve el path relativo, a la ubicacion del PAT, de un elemento,
 *  dependiendo de su tipo.
 * @param[in]	pElementNode: puntero a un nodo elemento
 * @param[in]	NroElem: nro. de elemento para generar path del directorio del elemento.
 * @param[in]	ContextName: propiedad "nombre" de nodo contexto
 * @param[in]	IsDirectory: flag para indicar si lo que se quiere generar es un
 * path de directorio o de elemento.
 * @return Ok? path asignado al elemento: "".
 * Precondicion: pElementNode debe ser un puntero a un nodo elemento no nulo.
 */
std::string PatFile::GetPathElement(const wxXmlNode *pElementNode,
		const int NroElem, const std::string &ContextName,
		const bool IsDirectory) const {
	if (ContextName.empty())
		return "";

	wxXmlProperty *pprop = pElementNode->GetProperties();
	while (pprop && (pprop->GetName().Cmp(wxT(TYPE_NODE)) != 0)) //El xml no se traduce
		pprop = pprop->GetNext();
	if (!pprop) {
		SET_ERROR_MSG = _(message_MISSING_TYPE_PROPERTY);
		REPORT_AND_FAIL_VALUE("D:Sin propiedad <tipo>.", "");
	}

	wxString elementype = pprop->GetValue().c_str();
	std::string path;
	path = ContextName + wxFileName::GetPathSeparator(wxPATH_UNIX) + elementype;

	if (!IsDirectory) {
		path += wxFileName::GetPathSeparator(wxPATH_UNIX);
		path += "Elem_";
		path += suri::LongToString(NroElem);
	}

	return path;
}

/**
 * Objetivo: Verifica que los nodos url del nodo dependencia sean validos.
 * @param[in]	VecDependency: vector de nodos dependencia.
 * @param[in]	List: lista de los archivos en el PAT.
 * @return	contenido de un nodo url no existe en la lista? false : true
 */
bool PatFile::CheckDependency(const std::vector<wxXmlNode*> VecDependency,
		const suri::ZipFile::ZipListType &List) const {
	// no hay nodos <dependencia>
	if (VecDependency.size() == 0)
		return true;
	switch (GetPatType()) {
	case PatCompressed: {
		for (size_t i = 0; i < VecDependency.size(); i++) {
			//obtengo el 1er nodo <url>
			wxXmlNode *pchild = VecDependency.at(i)->GetChildren();
			while (pchild) //leo todos los nodos <url> del nodo <dependencia>
			{
				if (pchild->GetName().Cmp(wxT(URL_NODE)) == 0) //El xml no se traduce
					if (!FindFileList(pchild->GetNodeContent().c_str(), List))
						return false;
				pchild = pchild->GetNext();
			}
		}
		break;
	}
	case PatRelative: {
		wxFileName fnxml(filePath_);
		for (size_t i = 0; i < VecDependency.size(); i++) {
			wxXmlNode *pnode = VecDependency.at(i)->GetChildren();
			while (pnode) {
				if (pnode->GetName().Cmp(wxT(URL_NODE)) == 0) //El xml no se traduce
						{
					wxFileName fnfile(
							fnxml.GetPath(wxPATH_GET_VOLUME, wxPATH_UNIX)
									+ fnxml.GetPathSeparator(wxPATH_UNIX)
									+ pnode->GetNodeContent().c_str());
					if (!fnfile.FileExists())
						REPORT_AND_FAIL_VALUE(
								"D:No existe el archivo: %s",
								false,
								filePath_.c_str(), fnfile.GetFullName().c_str());
				}
				pnode = pnode->GetNext();
			}
		}
		break;
	}
	case PatAbsolute: {
		for (size_t i = 0; i < VecDependency.size(); i++) {
			wxXmlNode *pnode = VecDependency.at(i)->GetChildren();
			while (pnode) {
				if (pnode->GetName().Cmp(wxT(URL_NODE)) == 0) //El xml no se traduce
						{
					wxFileName fnfile(pnode->GetNodeContent().c_str());
					if (!fnfile.FileExists())
						REPORT_AND_FAIL_VALUE(
								"D:No existe el archivo: %s",
								false,
								filePath_.c_str(), fnfile.GetFullName().c_str());
				}
				pnode = pnode->GetNext();
			}
		}
		break;
	}
	default:
		break;
	}
	return true;
}

/**
 * Objetivo: Devuelve un vector de nodos dependencia
 * @param[in]	VecElement: vector de nodos elemento
 * @return Devuelve un vector de nodos dependencia. En caso de error devuelve
 * un vector vacio.
 * \attention no deben ser eliminados los punteros del vector.
 */
std::vector<wxXmlNode*> PatFile::GetDependency(
		const std::vector<wxXmlNode*> VecElement) const {
	std::vector<wxXmlNode*> vecdependency;
	vecdependency.clear();

	if (VecElement.size() == 0)
		return vecdependency;

	for (size_t i = 0; i < VecElement.size(); i++) {
		wxXmlNode *pchild = VecElement.at(i)->GetChildren();
		while (pchild && pchild->GetName().Cmp(wxT(DEPENDENCY_NODE)) != 0) //El xml no se traduce
			pchild = pchild->GetNext();

		if (pchild)
			vecdependency.push_back(pchild);
	}

	return vecdependency;
}

/**
 * Objetivo: Modifica el path absoluto de las dependencias por un path relativo
 * al PAT.
 * @param[in]	pNode: nodo elemento.
 * @param[in]	Path: path del archivo dentro del PAT (sin incluir el archivo;
 * 	ej: Imagenes/Elem_0/xxx.tif o C:/Imagenes/xxx.tif).
 * @param[in]	PathUrl: path contenido en el nodo url del nodo elemento.
 * @param[out] pNode: nodo elemento con el nodo dependencia modificado.
 * @param[out]	DirList: lista con los directorios relativos, de los archivos.
 * @param[out]	FileList: lista con los path absolutos de los archivos a guardar
 * 	en el zip.
 */
void PatFile::ModifyDependency(wxXmlNode *pNode, const std::string &Path,
		const std::string &PathUrl, suri::ZipFile::ZipListType &FileList,
		suri::ZipFile::ZipListType &DirList) const {
	wxXmlNode *pnodedep = pNode->GetChildren();
	std::string filename;

	//Busco el nodo dependencias.
	while (pnodedep && pnodedep->GetName().Cmp(wxT(DEPENDENCY_NODE)) != 0) //El xml no se traduce
		pnodedep = pnodedep->GetNext();

	//si no existe el nodo dependencia salgo.
	if (!pnodedep)
		return;

	//obtengo el 1er nodo url de las dependencias.
	wxXmlNode *pnodeurl = pnodedep->GetChildren();
	//wxFileName fn;
	while (pnodeurl) {
		if (pnodeurl->GetName().Cmp(wxT(URL_NODE)) == 0) //El xml no se traduce
				{
			// Obtengo el contenido del nodo url para luego poder modificarlo
			wxXmlNode *pchild2 = pnodeurl->GetChildren();
			if (pchild2) {
				wxFileName fn(pnodeurl->GetNodeContent().c_str());
				wxFileName fnpathaux(Path);

				//Genero el path que va a tener la dependencia dentro del PAT
				wxString pathdir = fnpathaux.GetVolume().c_str()
						+ (fnpathaux.HasVolume() ?
								fnpathaux.GetVolumeSeparator() : wxT(""));
				pathdir += fnpathaux.GetFullPath(wxPATH_UNIX).c_str();

				//Genero el path (absoluto) del archivo para guardarlo en el PAT.
				wxString pathfile = wxT("");
				if (GetPatType() == PatAbsolute) {
					pathfile = fn.GetVolume().c_str()
							+ (fn.HasVolume() ?
									fn.GetVolumeSeparator() : wxT(""));
					pathfile += fn.GetFullPath(wxPATH_UNIX).c_str();
				} else //PatRelative
				{
					wxFileName fnaux(PathUrl);
					pathfile = fnaux.GetVolume().c_str()
							+ fnaux.GetVolumeSeparator();
					pathfile += fnaux.GetPath(wxPATH_GET_VOLUME, wxPATH_UNIX);
					wxString aux = wxT("");
					if (fn.GetFullPath(wxPATH_UNIX).StartsWith(
							wxT(Path).c_str(), &aux))
						pathfile += aux;
				}

				//concateno al path del directorio el path del elemento.
				if (pathfile != wxEmptyString) {
					wxString aux;
					if (pathfile.StartsWith(wxT(PathUrl).c_str(), &aux))
						pathdir += wxFileName::GetPathSeparator(wxPATH_UNIX)
								+ wxFileName(aux).GetPath(wxPATH_GET_VOLUME,
										wxPATH_UNIX);
				}

				FileList.push_back(pathfile.c_str());
				DirList.push_back(pathdir.c_str());

				//modifico el contenido del nodo url
				if (pathdir.Find(fn.GetPathSeparator(wxPATH_UNIX), true)
						!= (int) (pathdir.Len() - 1))
					pathdir = pathdir + fn.GetPathSeparator(wxPATH_UNIX);

				pchild2->SetContent(pathdir + fn.GetFullName().c_str());
			}
		}
		pnodeurl = pnodeurl->GetNext();
	}
}

/**
 *  Metodo auxiliar que cargar las htmlentities correspondientes para
 *  la conversion de textos de los pats
 */
void LoadHtmlEntities(std::map<std::string, std::string>& HtmlEntitiesMap) {
   HtmlEntitiesMap.insert(std::make_pair("&", "&amp;"));
   HtmlEntitiesMap.insert(std::make_pair("\223", "&quot;"));
   HtmlEntitiesMap.insert(std::make_pair("\224", "&quot;"));
   HtmlEntitiesMap.insert(std::make_pair("\"", "&quot;"));
   HtmlEntitiesMap.insert(std::make_pair("á", "&aacute;"));
   HtmlEntitiesMap.insert(std::make_pair("é", "&eacute;"));
   HtmlEntitiesMap.insert(std::make_pair("í", "&iacute;"));
   HtmlEntitiesMap.insert(std::make_pair("ó", "&oacute;"));
   HtmlEntitiesMap.insert(std::make_pair("ú", "&ucute;"));
   HtmlEntitiesMap.insert(std::make_pair("ä", "&auml;"));
   HtmlEntitiesMap.insert(std::make_pair("ë", "&euml;"));
   HtmlEntitiesMap.insert(std::make_pair("ï", "&iuml;"));
   HtmlEntitiesMap.insert(std::make_pair("ö", "&ouml;"));
   HtmlEntitiesMap.insert(std::make_pair("ü", "&uuml;"));
}

/**
 * Objetivo: parsea el contenido de un nodo en un string.
 * @param[in]	pNode: Nodo que contiene el dato.
 * @param[in]	Separator: string utilizado para formatear la cadena resultado.
 * @return string con el contenido del nodo.
 */
std::string PatFile::ParseNode(const wxXmlNode *pNode,
		const std::string &Separator) {
	std::string field;
	while (pNode) {
		if (pNode->GetChildren())
			field.append(pNode->GetNodeContent().c_str());
		else
			field.append(pNode->GetContent().c_str());

		if ((pNode = pNode->GetNext()))
			field.append(Separator);
	}
   /**
    * Fix para que se convierte de una forma estandard la informacion
    * que viene cargada en el pat con caracteres no ascii 127 (acentos,
    * comillas, etc). Se utiliza una variable estatica para evitar
    * cargar el mapa cada vez que se inicia el parseo
    */
   wxString fieldhtml = field;
   static std::map<std::string, std::string> htmlentitiesmap;
   if (htmlentitiesmap.empty()) {
      LoadHtmlEntities(htmlentitiesmap);
   }
   std::map<std::string, std::string>::const_iterator it = htmlentitiesmap.begin();
   for (; it != htmlentitiesmap.end(); ++it)
      fieldhtml.Replace(it->first.c_str(), it->second.c_str(), true);
	return (field == " " || field == "//") ? "" : fieldhtml.c_str();
}

/**
 * Objetivo: Agrega la imagen de preview del PAT a la lista de archivos que se
 * incluiran en el zip(.pat)
 * @param[in]	pNodeInfo: nodo informacion.
 * @param[out]	FileList: lista de archivos que se incluyen en el zip.
 * @param[out]	DirList: lista de directorios a crear en el zip.
 */
void PatFile::AddPreviewImg(wxXmlNode *pNodeInfo,
		suri::ZipFile::ZipListType &FileList,
		suri::ZipFile::ZipListType &DirList) const {
	if (!pNodeInfo)
		return;
	wxXmlNode *pnode = pNodeInfo;
	if (pnode->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //El xml no se traduce
		pnode = pnode->GetNext();

	if (!pnode)
		return;

	wxXmlNode *pchild = pnode->GetChildren();
	while (pchild && pchild->GetName().Cmp(wxT(PREVIEW_NODE)) != 0) //El xml no se traduce
		pchild = pchild->GetNext();

	if (!pchild)
		return;
	pchild = pchild->GetChildren();
	//obtengo el nodo url del nodo preview.
	while (pchild && pchild->GetName().Cmp(wxT(URL_NODE)) != 0)
		pchild = pchild->GetNext();

	if (!pchild)
		return;

	wxFileName fn(pchild->GetNodeContent().c_str());
	wxXmlNode *pnodeaux = pchild->GetChildren();
	if (!pnodeaux)
		return;
	if (GetPatType() == PatAbsolute) {
		if (!fn.FileExists())
			return;
		wxString pathaux = fn.GetVolume().c_str()
				+ (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
		pathaux += fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
				wxPATH_UNIX).c_str();
		pathaux += fn.GetFullName().c_str();
		FileList.push_back(pathaux.c_str());
	} else {
		//recreo el path absoluto.
		wxFileName fnpat(filePath_);
		wxString pathpreview = fnpat.GetVolume().c_str()
				+ (fnpat.HasVolume() ? fnpat.GetVolumeSeparator() : wxT(""));
		pathpreview += fnpat.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
				wxPATH_UNIX).c_str();
		pathpreview += fn.GetFullName();
		if (!fn.FileExists(pathpreview))
			return;
		FileList.push_back(pathpreview.c_str());
	}
	//convierto el path del preview a relativo al PAT.
	pnodeaux->SetContent(fn.GetFullName().c_str());
	DirList.push_back(fn.GetFullName().c_str());
}

/**
 * Objetivo: Descomprime y devuelve los path del archivo descripcion.xml y de la
 * imagen de vista previa del PAT.
 * @param[in]	PathPat: path del Pat. Puede ser el path del PAT comprimido o el
 * path del archivo descripcion.xml.
 * @param[out]	PathXml: path del archivo descripcion.xml
 * @param[out]	PathImg: path de la imagen de vista previa del PAT.
 */
void PatFile::GetPaths(const std::string &PathPat, std::string &PathXml,
		std::string &PathImg) {
	PathXml = PathImg = "";
	wxFileName fn(PathPat);
	if (GetPatType() == PatCompressed) {
		suri::ZipFile zip(PathPat);
		//Creo archivo temporal en la carpeta temp del SO
		std::string pathtempxml;
		pathtempxml = fn.GetTempDir() + fn.GetPathSeparator() + "pat.xml";
		std::string auxpathxml = fn.CreateTempFileName(pathtempxml).c_str();
		if (auxpathxml.length() == 0) {
			SetErrorMsg(_(message_TEMP_FILE_CREATION_ERROR));
			return;
		}

		/*descomprimo el .xml en un archivo temporal*/
		if (!zip.Extract("descripcion.xml", auxpathxml, false, true)) {
			SetErrorMsg(_(message_PAT_DESCRIPTION_EXTRACTION_ERROR));
			return;
		}

		//Cargo el xml para poder descomprimir la imagen del preview.
		wxXmlDocument docxml;
		if (!docxml.Load(auxpathxml, suri::XmlElement::xmlEncoding_)) {
			SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
			return;
		}

		//Obtengo nodo informacion.
		wxXmlNode *pchild = docxml.GetRoot()->GetChildren();
		while (pchild && pchild->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //El xml no se traduce
			pchild = pchild->GetNext();

		if (!pchild)
			return;
		wxXmlNode *pchild2 = pchild->GetChildren();
		while (pchild2 && pchild2->GetName().Cmp(wxT(PREVIEW_NODE)) != 0) //El xml no se traduce
			pchild2 = pchild2->GetNext();

		if (!pchild2)
			return;

		wxXmlNode *pchild3 = pchild2->GetChildren();
		while (pchild3 && pchild3->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
			pchild3 = pchild3->GetNext();

		if (!pchild3)
			return;
		std::string auxpathimg = "";
		if (!pchild3->GetNodeContent().IsEmpty()) {
			//descomprimo la imagen de preview en un archivo temporal.
			wxFileName fnimg(pchild3->GetNodeContent().c_str());
			std::string pathtempimg;
			pathtempimg = fnimg.GetTempDir() + fnimg.GetPathSeparator()
					+ fnimg.GetFullName();
			auxpathimg = fn.CreateTempFileName(pathtempimg).c_str();
			if (auxpathimg.length() == 0) {
				SetErrorMsg(_(message_TEMP_FILE_CREATION_ERROR));
				return;
			}

			if (!zip.Extract(pchild3->GetNodeContent().c_str(), auxpathimg,
					false, true)) {
				SetErrorMsg(_(message_PAT_PREVIEW_EXTRACTION_ERROR));
				return;
			}
		}

		wxFileName fnxml(auxpathxml);
		PathXml = fnxml.GetVolume().c_str()
				+ (fnxml.HasVolume() ? fnxml.GetVolumeSeparator() : wxT(""))
				+ fnxml.GetFullPath(wxPATH_UNIX).c_str();
		wxFileName fnimg(auxpathimg);
		PathImg = fnimg.GetVolume().c_str()
				+ (fnimg.HasVolume() ? fnimg.GetVolumeSeparator() : wxT(""))
				+ fnimg.GetFullPath(wxPATH_UNIX).c_str();
		return;
	}

	//No es un PAT comprimido
	wxXmlDocument docxml;
	if (!docxml.Load(fn.GetFullPath().c_str(), suri::XmlElement::xmlEncoding_)) //cargo xml
			{
		SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
		return;
	}

	//Obtengo nodo informacion.
	wxXmlNode *pchild = docxml.GetRoot()->GetChildren();
	while (pchild && pchild->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //El xml no se traduce
		pchild = pchild->GetNext();

	if (!pchild)
		return;
	wxXmlNode *pchild2 = pchild->GetChildren();
	while (pchild2 && pchild2->GetName().Cmp(wxT(PREVIEW_NODE)) != 0) //El xml no se traduce
		pchild2 = pchild2->GetNext();

	if (!pchild2)
		return;

	PathXml = fn.GetVolume().c_str()
			+ (fn.HasVolume() ? fn.GetVolumeSeparator() : "");
	PathXml += fn.GetFullPath(wxPATH_UNIX);

	pchild2 = pchild2->GetChildren();
	while (pchild2 && pchild2->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		pchild2 = pchild2->GetNext();

	PathImg = "";
	if (pchild2 && !pchild2->GetNodeContent().IsEmpty()) {
		wxFileName fnimg(pchild2->GetNodeContent().c_str());

		if (GetPatType() == PatRelative) {
			PathImg = fn.GetVolume().c_str()
					+ (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
			PathImg += fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
					wxPATH_UNIX);
			PathImg += fnimg.GetFullName();
			return;
		}

		PathImg = fnimg.GetVolume().c_str()
				+ (fnimg.HasVolume() ? fnimg.GetVolumeSeparator() : "");
		PathImg += fnimg.GetFullPath(wxPATH_UNIX);
	}

	return;
}

/**
 * Objetivo: Modifica el nodo url del nodo textura para que cuando se instale
 * el PAT, el terreno pueda ubicar la textura correctamente.
 * @param[in]	pNode: nodo elemento
 * @param[in]	Path: path relativo al PAT.
 * @param[in]	PathUrl: path contenido en el nodo url del nodo elemento.
 */
void PatFile::ModifyTextureNode(wxXmlNode *pNode, const std::string &Path,
		const std::string &PathUrl) const {
	if (!pNode)
		return;

	//Busco nodo RENDERIZACION
	wxXmlNode *pchild = pNode->GetChildren();
	while (pchild && pchild->GetName().Cmp(wxT(RENDERIZATION_NODE)) != 0) //El xml no se traduce
		pchild = pchild->GetNext();

	if (!pchild)
		return;

	//Busco nodo Textura
	wxXmlNode *pnodetexture = pchild->GetChildren();
	while (pnodetexture && pnodetexture->GetName().Cmp(wxT(TEXTURE_NODE)) != 0) //El xml no se traduce
		pnodetexture = pnodetexture->GetNext();

	if (!pnodetexture)
		return;

	//Busco el nodo elemento
	wxXmlNode *pnodeelement = pnodetexture->GetChildren();
	while (pnodeelement
			&& pnodeelement->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)) != 0) //El xml no se traduce
		pnodeelement = pnodeelement->GetNext();

	if (!pnodeelement)
		return;

	//busco nodo url y reemplazo contenido
	wxXmlNode *pnodeurl = pnodeelement->GetChildren();
	while (pnodeurl && pnodeurl->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		pnodeurl = pnodeurl->GetNext();

	if (!pnodeurl)
		return;

	wxFileName fn(pnodeurl->GetNodeContent().c_str());
	wxFileName fnpath(Path);
	wxString pathaux = fnpath.GetVolume().c_str()
			+ (fnpath.HasVolume() ? fnpath.GetVolumeSeparator() : wxT(""));
	pathaux += fnpath.GetFullPath(wxPATH_UNIX).c_str();

	wxString pathfind = fn.GetVolume().c_str()
			+ (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""))
			+ fn.GetFullPath(wxPATH_UNIX);
	if (pathfind != wxEmptyString) {
		wxString aux;
		if (pathfind.StartsWith(wxT(PathUrl).c_str(), &aux))
			pathaux += wxFileName::GetPathSeparator(wxPATH_UNIX)
					+ wxFileName(aux).GetPath(
							wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
							wxPATH_UNIX);
	}

	if (pathaux.Find(fn.GetPathSeparator(wxPATH_UNIX), true)
			!= (int) (pathaux.Len() - 1))
		pathaux += fn.GetPathSeparator(wxPATH_UNIX);

	pnodeurl->GetChildren()->SetContent(pathaux + fn.GetFullName().c_str());
}

/**
 * Objetivo: Obtiene los url de los hotlinks para poder incluirlos en el .pat.
 * @param[in]	pNode: nodo elemento.
 * @param[out]	FileList: lista con url's de archivos a agregar al zip.
 * @param[out]	DirList: lista con path dentro del zip de dichos archivos.
 */
void PatFile::AddHotlinks(const wxXmlNode *pNode,
		suri::ZipFile::ZipListType &FileList,
		suri::ZipFile::ZipListType &DirList) const {
	/* El wxFileName(filePath_) es para poder recrear el url del vector, ya que
	 * en caso de no hacerlo, falla la creacion.
	 */
	wxFileName fnpat(filePath_);
	wxString pathaux = wxT("");
	if (GetPatType() == PatRelative)
		pathaux = fnpat.GetPath() + fnpat.GetPathSeparator();
	if (!pNode || !IsHotLinkVectorElement(pNode, pathaux))
		return;

	//si no es un elemento tipo vector salgo.
	if (pNode->GetPropVal(wxT(TYPE_PROPERTY), wxT("")).Cmp(
			wxT(TYPE_PROPERTY_VALUE_VECTOR)) != 0) //El xml no se traduce
		return;

	/* obtengo nodo url para crear vector y modificar el .dbf; dejando un path
	 * relativo al hotlink
	 */
	wxXmlNode *purlnode = pNode->GetChildren();
	while (purlnode && purlnode->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		purlnode = purlnode->GetNext();
	if (!purlnode)
		return;

	// armo el url del vector para que no falle la creacion.
	wxString pathvector = pathaux + purlnode->GetNodeContent().c_str();
	HotLinkVectorElement *phlelement = HotLinkVectorElement::Create(
			pathvector.c_str());
	if (!phlelement)
		return;

	std::string relativepath = suri::Configuration::GetParameter(
			"pat_common_dir", wxT(""));
	relativepath += suri::Configuration::GetParameter("app_hotlinks_dir",
			wxT(""));

	wxFileName fnhl(phlelement->GetHotLinkFieldValue());
	/* obtengo el url del html para quedarme solo con el path, que es el mismo
	 * que el de la imagen, para poder incluir dichos archivos en el PAT.*/
	wxString hlelementspath = fnhl.GetVolume()
			+ (fnhl.HasVolume() ? fnhl.GetVolumeSeparator() : wxT(""))
			+ fnhl.GetPathWithSep(wxPATH_UNIX);

	std::string newurlhotlink = relativepath;
	if (fnhl.GetDirCount() > 0) {
		newurlhotlink += fnhl.GetDirs().Item(fnhl.GetDirCount() - 1).c_str();
		newurlhotlink += fnhl.GetPathSeparator(wxPATH_UNIX);
	}
	newurlhotlink += fnhl.GetFullName().c_str();

	phlelement->SetHotLinkFieldValue(newurlhotlink);
	delete phlelement;

	//Busco nodo caracteristicas
	wxXmlNode *pnodefeatures = pNode->GetChildren();
	while (pnodefeatures
			&& pnodefeatures->GetName().Cmp(wxT(FEATURES_NODE)) != 0) //El xml no se traduce
		pnodefeatures = pnodefeatures->GetNext();
	if (!pnodefeatures)
		return;

	//Agrego url de la imagen del html.
	wxXmlNode *pnodeurl = pnodefeatures->GetChildren();
	while (pnodeurl && pnodeurl->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		pnodeurl = pnodeurl->GetNext();
	if (!pnodeurl)
		return;

#if 0
	//genero el path absoluto para prependearlo al path de los archivos.
	wxString pathpat = fnpat.GetVolume().c_str() +
	(fnpat.HasVolume()?fnpat.GetVolumeSeparator():wxT(""));
	pathpat += fnpat.GetPath(wxPATH_GET_VOLUME|
			wxPATH_GET_SEPARATOR, wxPATH_UNIX).c_str();
	pathpat += relativepath;
#endif

	if (!pnodeurl->GetNodeContent().IsEmpty()) {
		//obtengo el path de la imagen.
		wxFileName fnimg(pnodeurl->GetNodeContent());
		wxString pathimg = hlelementspath + fnimg.GetFullName();
		if (!wxFileName::FileExists(pathimg))
			return;
		FileList.push_back(pathimg.c_str());
		DirList.push_back(relativepath + fnimg.GetPath().c_str());
	}

	//Agrego url del html.
	wxXmlNode *pnodehtml = pnodefeatures->GetChildren();
	while (pnodehtml && pnodehtml->GetName().Cmp(wxT(HTML_NODE)) != 0) //El xml no se traduce
		pnodehtml = pnodehtml->GetNext();
	if (!pnodehtml)
		return;
	//obtengo el nodo url del nodo html
	wxXmlNode *pnodehtmlurl = pnodehtml->GetChildren();
	while (pnodehtmlurl && pnodehtmlurl->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		pnodehtmlurl = pnodehtmlurl->GetNext();
	if (!pnodehtmlurl)
		return;
	if (!pnodehtmlurl->GetNodeContent().IsEmpty()) {
		//obtengo el path del html
		wxFileName fnhtml(pnodehtmlurl->GetNodeContent());
		wxString htmlpath = hlelementspath + fnhtml.GetFullName();
		if (!fnhtml.FileExists(htmlpath))
			return;
		FileList.push_back(htmlpath.c_str());
		DirList.push_back(relativepath + fnhtml.GetPath().c_str());
	}
}

/**
 * Objetivo:	Modifica el contenido del campo SURHotlink de todos los vectores,
 * prependeando el path donde se instalo el PAT.
 * @param[in]	XmlUrl: url del archivo descripcion.xml
 * @return modifica_ok? true : false;
 */
bool PatFile::ModifyHotLinks(const wxString &XmlUrl) const {
	if (XmlUrl.IsEmpty())
		return false;

	wxXmlDocument doc;
	if (!doc.Load(XmlUrl, suri::XmlElement::xmlEncoding_))
		return false;

	wxFileName fn(XmlUrl);
	std::string commonpath = suri::Configuration::GetParameter(
			wxT("pat_common_dir"), wxT("./"));
	commonpath += suri::Configuration::GetParameter(wxT("app_vector_dir"),
			wxT(""));

	// obtengo el nodo contexto y busco el contexto raster
	wxXmlNode *pcontextnode = doc.GetRoot()->GetChildren();
	while (pcontextnode) {
		wxString contextprop = pcontextnode->GetPropVal(wxT(TYPE_PROPERTY), "");
		if (contextprop == wxT(TYPE_PROPERTY_VALUE_RASTER)
				|| contextprop == wxT(TYPE_PROPERTY_VALUE_TERRAIN)) //El xml no se traduce
				{
			wxString contextname = pcontextnode->GetPropVal(wxT(NAME_PROPERTY),
					""); //El xml no se traduce
			// recorro los elemento vector
			wxXmlNode *pelementnode = pcontextnode->GetChildren();
			for (; pelementnode; pelementnode = pelementnode->GetNext()) {
				if (pelementnode->GetPropVal(wxT(TYPE_PROPERTY), "").Cmp(
						wxT(TYPE_PROPERTY_VALUE_VECTOR)) != 0) //El xml no se traduce
					continue;

				wxXmlNode *purlnode = pelementnode->GetChildren();
				while (purlnode && purlnode->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
					purlnode = purlnode->GetNext();

				if (!purlnode)
					return false;

				wxString vectorurl = wxT("");
				if (GetPatType() != PatAbsolute)
					vectorurl = fn.GetVolume()
							+ (fn.HasVolume() ?
									fn.GetVolumeSeparator() : wxT(""))
							+ fn.GetPath(
									wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
									wxPATH_UNIX);

				vectorurl += purlnode->GetNodeContent();
				HotLinkVectorElement *phlelement = HotLinkVectorElement::Create(
						vectorurl.c_str());
				if (!phlelement)
					continue; //seria una anotacion.

				wxString hotlinkurl = phlelement->GetHotLinkFieldValue();
				int pos =
						vectorurl.Find(contextname) == wxNOT_FOUND ?
								vectorurl.Find(wxString(commonpath)) :
								vectorurl.Find(contextname);
				if (pos == wxNOT_FOUND)
					return false; //no deberia pasar.
				hotlinkurl.Prepend(vectorurl.Mid(0, pos));
				pos = wxNOT_FOUND;
				phlelement->SetHotLinkFieldValue(hotlinkurl.c_str());
				delete phlelement;
			}
		}
		pcontextnode = pcontextnode->GetNext();
	}

	return true;
}

/**
 * Objetivo: Setear el error de mensaje resultante de una operacion.
 * @param[in]	Msg: cadena con el mensaje de error.
 */
void PatFile::SetErrorMsg(const std::string &Msg) {
	errorMsg_.clear();
	errorMsg_ = Msg;
}

/**
 * Objetivo: Verifica que el nodo Fecha tenga solo 3 nodos(Dia, Mes, Anio)
 * @param[in]	pDateNode: puntero al nodo Fecha.
 * @return nodo_fecha_correcto? true:false.
 */
bool PatFile::VerifyDateNode(const wxXmlNode *pDateNode) {
	if (!pDateNode)
		return false;
	wxXmlNode *pnode = pDateNode->GetChildren();
	while (pnode)
		if (pnode->GetName().Cmp(DAY_NODE) == 0
				|| pnode->GetName().Cmp(MONTH_NODE) == 0
				|| pnode->GetName().Cmp(YEAR_NODE) == 0)
			pnode = pnode->GetNext();
		else
			return false;

	return true;
}

/**
 * Agrega el tag HTML de nueva linea <BR> para obtener lineas de a lo sumo
 * Width caracteres sin cortar palabras.
 * \post Description posee el tag <BR> cada Width caracteres o menos.
 * @param[in]	Description: cadena a formatear.
 * @param[in]	Width: ancho de cada linea.
 * @return cadena formateada.
 */
std::string PatFile::FormatDescription(const std::string &Description,
		const int &Width) {
	if (Description.empty())
		return _(content_NO_DESCRIPTION);
	if (Width == 0)
		return Description;
	std::string text;
	text.clear();
	if ((int) (Description.length()) > Width) {
		int lines = (Description.length() / Width); //es la cantidad de <br> que le agrego a la cadena.
		char *ptoken, *pdescription = new char[Description.length() + lines];memset
		(pdescription, '\0', Description.length() + lines);
		std::string line;
		Description.copy(pdescription, Description.length());
		for (ptoken = strtok(pdescription, " "); ptoken;
				ptoken = strtok(NULL, " "))
			if ((int) (line.length() + (strlen(ptoken) - 1)) < Width) {
				line += (ptoken);
				line += " ";
			} else {
				line += "<br>";
				text += line;
				if (strlen(ptoken) != 0)
					text += ptoken;
				text += " ";
				line.clear();
			}
		if (!line.empty())
			text += line;
		delete[] pdescription;
	}

	if (text.empty())
		return Description;
	return text.c_str();
}

/**
 * Objetivo: Obtiene el contenido del nodo informacion|nombre y lo asigna a la
 * propiedad fileName_.
 *
 * \remarks Resulta confuso el uso del parametro PathXml ya que toma un XML que podria
 *      no ser el mismo que esta representado por la instancia y generar
 *      inconsistencias con los metodos y propiedades que se obtuvieron con
 *      anterioridad.
 * @param[in]	PathXml: solo usada cuando es un PAT comprimido, contiene el path
 * del archivo descripcion xml.
 */
void PatFile::SetPatName(const wxString &PathXml) {
	PatType type = GetPatType();
	if (type == PatInvalid)
		fileName_ = wxT("");
   std::string encoding = suri::XmlElement::xmlEncoding_;
#ifdef __LINUX__
   encoding = suri::XmlElement::utf8XmlEncoding_;
#endif
	if (type == PatCompressed) {
		if (PathXml.IsEmpty())
			REPORT_AND_FAIL("D:No existe el archivo: %s", PathXml.c_str());
		wxFileName fn(PathXml);
		wxString pathaux = fn.GetVolume().c_str()
				+ (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""))
				+ fn.GetFullPath(wxPATH_UNIX);
		//Obtengo el contenido del nodo informacion/nombre.
		wxXmlDocument doc;
		if (doc.Load(pathaux, encoding)) {
			wxXmlNode *pnode = doc.GetRoot()->GetChildren();
			//busco el nodo informacion
			while (pnode && pnode->GetName().Cmp(INFORMATION_NODE) != 0) //El xml no se traduce
				pnode = pnode->GetNext();
			//Busco el nodo nombre
			if (pnode)
				pnode = pnode->GetChildren();
			while (pnode && pnode->GetName().Cmp(NAME_NODE) != 0) //El xml no se traduce
				pnode = pnode->GetNext();
			if (pnode)
				fileName_ = pnode->GetNodeContent().c_str();
		}
	} else {
		wxFileName fn(filePath_);
		wxString pathxml = fn.GetVolume()
				+ (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
		pathxml += fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
				wxPATH_UNIX);
		pathxml += fn.GetFullName().c_str();
		wxXmlDocument docxml;
		if (!docxml.Load(pathxml, encoding)) {
			SET_ERROR_MSG = _(message_XML_LOAD_ERROR);
			REPORT_AND_FAIL("D:Error al cargar archivo xml.");
		}

		//Obtengo nodo informacion.
		wxXmlNode *pchild =
				docxml.GetRoot() ? docxml.GetRoot()->GetChildren() : NULL;
		while (pchild && pchild->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //El xml no se traduce
			pchild = pchild->GetNext();

		if (pchild)
			pchild = pchild->GetChildren();

		while (pchild && pchild->GetName().Cmp(wxT(NAME_NODE)) != 0) //El xml no se traduce
			pchild = pchild->GetNext();

		if (pchild)
			fileName_ = pchild->GetNodeContent().c_str();
	}
}

/**
 * Devuelve todos los nodos elemento de un PAT.
 * @param[in]	PathXml path absoluto del archivo descripcion.xml.
 * @param[in]	AbsolutePath flag para que devuelva los elementos con urls absolutos.
 * @param[out]	VecElementsNode vector de nodos elemento.
 * \todo(12/06/2009 - Alejandro) buscar y reemplazar logica similar.
 */
void PatFile::GetElementsNode(std::vector<wxXmlNode*> &VecElementsNode,
		const std::string &PathXml, const bool AbsolutePath) const {
	VecElementsNode.clear();
	std::map < std::string, std::vector<wxXmlNode*> > contextmap;
	contextmap = GetContextNodes(PathXml.empty() ? filePath_ : PathXml);
	std::map<std::string, std::vector<wxXmlNode*> >::iterator it;
	for (it = contextmap.begin(); it != contextmap.end(); it++)
		VecElementsNode.insert(VecElementsNode.end(), it->second.begin(),
				it->second.end());

	if (typePat_ == PatRelative && AbsolutePath) {
		std::vector<wxXmlNode*>::iterator itvec = VecElementsNode.begin();
		for (; itvec != VecElementsNode.end(); itvec++) {
			GenericMakeUrlAbsolute(*itvec,
					PathXml.empty() ? filePath_ : PathXml);
			GenericMakeUrlAbsolute(GetDependencyNode(*itvec),
					PathXml.empty() ? filePath_ : PathXml);
		}
	}
}

//--------------------------------------------
/**
 * Devuelve el tamanio de los archivos que conforma cada nodo elemento.
 * Crea un elemento generico, reemplaza su nodo por el nodo elemento que trae el
 * vector de nodos elemento, obtiene su tamanio y lo suma.
 * @param[in]	VecElementNode vector de nodos elemento.
 * @return tamanio total de los archivos descriptos en los nodos elemento.
 */
wxULongLong GetFilesSize(std::vector<wxXmlNode *> VecElementNode) {
	wxULongLong size = 0, sizeaux = 0;
	std::vector<wxXmlNode *>::iterator it = VecElementNode.begin();
	// para que al linkear agregue a MemoryVectorElement.o, sino no se auto-registra
	delete new suri::MemoryVectorElement;
	for (; it != VecElementNode.end(); it++) {
		// para que sea incluido en el linkeo
		delete new suri::MemoryVectorElement;
		// cabeza pero anda (porque Element es virtual puro)
		suri::Element *pelement = new suri::MemoryVectorElement();
		if (!pelement)
			return wxInvalidSize;
		pelement->AddNode(pelement->GetNode(CONTEXT_ELEMENT_NODE), *it, true);
		sizeaux = pelement->GetSize();
		suri::Element::Destroy(pelement);

		if (sizeaux != wxInvalidSize)
			size += sizeaux;
	}
	return size;
}
/**
 * Objetivo: Verifica si el elemento es un HotLink.
 * @param[in]	Path: es el path del directorio donde se encuentra el hotlink.
 * @param[in]	pElementNode: nodo elemento.
 * @return es HotLink? true : false.
 */
bool IsHotLinkVectorElement(const wxXmlNode *pElementNode,
		const wxString &Path) {
	if (!pElementNode)
		return false;

	if (pElementNode->GetPropVal(wxT(TYPE_PROPERTY), "").Cmp(
			wxT(TYPE_PROPERTY_VALUE_VECTOR)) != 0) //El xml no se traduce
		return false;

	wxXmlNode *pchildnode = pElementNode->GetChildren();
	while (pchildnode && pchildnode->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		pchildnode = pchildnode->GetNext();
	if (!pchildnode)
		return false;

	wxString pathvector = Path + pchildnode->GetNodeContent().c_str();
	HotLinkVectorElement *phlelement = HotLinkVectorElement::Create(
			pathvector.c_str());
	wxString fieldname = wxT("");
	if (!phlelement || phlelement->GetHotLinkFieldName(0, fieldname).IsEmpty())
		return false;

	delete phlelement;
	return true;
}

/**
 * Verifica si el elemento corresponde a un archivo temporal, es decir un elemento
 * que se encuentra en la carpeta temporal del sistema.
 * @param[in]	pElementNode: puntero al nodo Elemento.
 * @return true en caso de tratarse de un archivo temporal.
 */
bool IsTempFile(const wxXmlNode *pElementNode) {
	if (!pElementNode)
		return false;

	wxXmlNode *pchildnode = pElementNode->GetChildren();
	while (pchildnode && pchildnode->GetName().Cmp(wxT(URL_NODE)) != 0) //El xml no se traduce
		pchildnode = pchildnode->GetNext();
	if (!pchildnode)
		return false;

	std::string tempdir = suri::Configuration::GetParameter("app_temp_dir",
			wxT(""));

	return (!pchildnode->GetNodeContent().IsEmpty()
			&& pchildnode->GetNodeContent().StartsWith(tempdir.c_str()));
}

/**
 * Modifica el contenido del nodo/s url para que sea absoluto.
 * @param[in]	pNode nodo elemento
 * @param[in]	AbsolutePath Path para prependear al url del elemento para hacer
 * el url del elemento absoluto.
 * @return true si pudo modificar el url del elemento.
 * @return false si no pudo modificar el url del elemento.
 */
bool MakeUrlAbsolute(wxXmlNode *pNode, std::string AbsolutePath) {
	if (!pNode)
		return false;

	bool iselementnode = (pNode->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)) == 0); //No se traduce, elemento XML
	wxXmlNode *pchild = pNode->GetChildren();
	while (pchild) {
		while (pchild->GetName().Cmp(wxT(URL_NODE)) != 0) //No se traduce, elemento XML
			pchild = pchild->GetNext();

		if (!pchild)
			return false;

		wxFileName fnpat(AbsolutePath);
		wxString url = fnpat.GetVolume().c_str()
				+ (fnpat.HasVolume() ? fnpat.GetVolumeSeparator() : wxT(""));
		url += fnpat.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
				wxPATH_UNIX);
		url += pchild->GetNodeContent().c_str();

		wxXmlNode *pchild2 = pchild->GetChildren();
		if (!pchild2)
			return false;
		pchild2->SetContent(url);

		// esto es por si pNode es un nodo "dependencias".
		if (!iselementnode)
			pchild = pchild->GetNext();
		else
			return true;
	}
	return true;
}

/**
 * Modifica el contenido del nodo/s url para que sea absoluto. Este funcion debe
 * ser llamada si se desconoce el tipo de elemento o si es un elemento de tipo
 * Terreno.
 * @param[out]	pNode: nodo elemento.
 * @param[in]	AbsolutePath Path para prependear al url del elemento para hacer
 * el url del elemento absoluto.
 * @return true si pudo modificar el url del elemento.
 * @return false si no pudo modificar el url del elemento.
 */
bool GenericMakeUrlAbsolute(wxXmlNode *pNode, std::string AbsolutePath) {
	if (!pNode || AbsolutePath.empty())
		return false;

	wxString type = wxT("");
	pNode->GetPropVal(wxT(TYPE_PROPERTY), &type); //No se traduce, elemento XML

	//Si no es terreno llama a MakeUrlAbsolute
	if (type.Cmp(wxT(TYPE_PROPERTY_VALUE_TERRAIN)) != 0) //No se traduce, elemento XML
		return MakeUrlAbsolute(pNode, AbsolutePath);
	//Si es terreno, convierte el elemento, busca el nodo URL de la textura y luego llama
	else {
		//convierto a absoluto el url del elemento Terreno.
		if (!MakeUrlAbsolute(pNode, AbsolutePath))
			return false;

		//Busco el nodo url del nodo textura
		wxXmlNode *pchild = pNode->GetChildren();
		while (pchild && pchild->GetName().Cmp(wxT(RENDERIZATION_NODE)) != 0) //No se traduce, elemento XML
			pchild = pchild->GetNext();
		if (!pchild)
			return false;
		pchild = pchild->GetChildren();
		while (pchild && pchild->GetName().Cmp(wxT(TEXTURE_NODE)) != 0) //No se traduce, elemento XML
			pchild = pchild->GetNext();
		if (!pchild)
			return false;
		pchild = pchild->GetChildren();
		while (pchild && pchild->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)) != 0) //No se traduce, elemento XML
			pchild = pchild->GetNext();
		if (!pchild)
			return false;
		//convierto a absoluto el url de la textura del terreno.
		return MakeUrlAbsolute(pchild, AbsolutePath);
	}
	return false;
}
