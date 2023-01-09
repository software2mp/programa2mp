//<COPYRIGHT> a reemplazar a futuro

#ifndef PATINFOPART_H_
#define PATINFOPART_H_

//Includes standard

//Includes Suri
#include "suri/Part.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/xml/xml.h"

//Includes App
#include "Context.h"
#include "suri/DataViewManager.h"

//Defines

DECLARE_EVENT_OBJECT(PatInfoPartEvent);

#define XMLMODE	"XmlContextHandler"

//! Clase para editar propiedades del PAT.
/**
 * Presenta una ventana en la cual se pueden editar
 * las propiedades del PAT(nombre, descripcion, autor, copyright, imagen previa).
 * Este Part, es utilizado para:
 * 	- crear un PAT vacio, solo editando su nombre.(Unicamente desde el
 * Administrador de PATs)
 * 	- editar las propiedades de un PAT guardadoa desde el Administrador de PATs.
 * 	- Salvar el contenido de los contextos generando un PAT nuevo o
 * sobreescribiendo el contenido de uno existente.
 * \todo Esta clase solo deberia proveer una GUI para la consulta/modificacion
 *       de los datos de informacion del PAT. En lugar de eso, tiene funcionalidad
 *       duplicada de otras clases y funcionalidad que pertenece a otras clases.
 */
class PatInfoPart: public suri::Part {
	//! Ctor. de Copia.
	PatInfoPart(const PatInfoPart &PatInfoPart);

public:
	FRIEND_EVENT_OBJECT(PatInfoPartEvent);
	//! Ctor.
	PatInfoPart(const wxXmlNode *pNodeInfo, std::list<Context*> *pContextList,
			const bool &IsUserPat, suri::DataViewManager* pDataviewmanager,
			const wxString &OldPatName = "", const bool &IsEditing = false,
			const bool &UpdateMainToolData = false);
	//! Dtor.
	~PatInfoPart();
	//! Se crea la ventana de herramienta en las clases derivadas
	virtual bool CreateToolWindow();
	//! Inicializa los controles
	virtual void SetInitialValues();
	//! Devuelve el estado del Part.
	virtual bool IsEnabled() {
		return true;
	}
	//! Indica si el Part tiene cambios para salvar.
	virtual bool HasChanged();
	//! Salva los cambios realizados en el Part.
	virtual bool CommitChanges();
	//! Restaura los valores originales del Part.
	virtual bool RollbackChanges();
	//! Responde al evento de click sobre el boton de dialogo de apertura de arch.
	void OnButtonPreview(wxCommandEvent &ButtonEvent);
	//! Devuelve una copia del Nodo Informacion.
	wxXmlNode *GetInfoNode();
	//! Devuelve el contenido del archivo descripcion.xml en un string.
	bool CreatePat(wxString &DescXml, const wxXmlNode *pInfoNode = NULL);
	//! Elmina archivos y directorios
	bool DeleteFiles(const wxString &DirPath, wxString &ErrorMsg);
	//! Crea el directorio que contiene al PAT y salva el archivo
	bool SaveXml(const wxString &PathPat, const wxString &XmlDesc,
			const wxString &OldNamePat = wxEmptyString);
	//! Establece flag indicando si el Part esta o no modificado.
	void SetModified(const bool &Modified) {
		modified_ = Modified;
	}
	//! Actualiza el estado de la parte
	virtual void Update() {
	}
protected:
private:
	//! Crea el nodo Informacion a partir de lo ingresado por el usuario.
	wxXmlNode *CreateInfoNode();
	//! Devuelve el contenido del nodo Informacion.
	void GetNodeValues(wxString &Name, wxString &Author, wxString &Copyright,
			wxString &Description, wxString &Preview);
	//! Devuelve los valores ingresados por el usuario y la fecha actual.
	void GetUsrValues(wxString &Name, wxString &Author, wxString &Copyright,
			wxString &Description, wxString &Preview, wxString &Day,
			wxString &Month, wxString &Year);
	//! Devuelve el nombre del PAT.
	wxString GetPatName(const wxXmlNode *pNode);
	//! Genera y devuelve el xml del PAT
	wxXmlNode * GetNewPat(const wxXmlNode *pInfoNode = NULL);
	//! Obtiene los Subdirectorios de un PAT.
	void GetSubDirectories(const wxArrayString *pVecItems,
			const wxString PathPat, wxArrayString *pVecDirs);
	//! Obtiene los caracters validos para generar un nombre de directorio.
	void GetValidChars(wxArrayString &Chars);
	//! Verifica la existencia de directorios con el nombre pasado en PatName
	bool CheckPatName();
	//! Verifica la existencia de directorios con el nombre pasado en PatName
	int CheckPatName(const wxString &PatName, const wxString &OldName,
			wxString &ErrorMsg);
	//! Agrega los contextos al xml.
	bool AddContexts(wxXmlNode *pNode, const bool &EmptyContexts = false);
	//! Elimina el contenido del nodo nombre.
	bool DeletePatName();

	/**
    * Devuelve la version de la aplicacion con la profundidad indicada.
    */
   std::string GetAppVersion(int Depth = 2) const;

	//! Genera y devuelve el nodo PAT.
	wxXmlNode *CreatePatNode();
	//! Verifica que la cadena no se encuentre en el vactor.
	bool CanInsert(const wxString Cad, const wxArrayString *Vec);
	//! Modifica los url de los vectores.
	void ModifyUrlVector(const std::string& PathPat,
			const std::string &CommonPath);
	//! Modifica el contenido del nodo url.
	wxString ModifyUrlNode(wxXmlNode *pUrlNode, const wxString &CommonPath);
	//! Modifica el contenido de los nodo url del nodo dependencias.
	void ModifyDependency(wxXmlNode *pNodeDep, const wxString &CommonPath);
	//! Modifica la base de datos del vector, indicando el nuevo path del hotlink.
	void ModifyHotLinkFieldValue(const wxString &CommonPath,
			const wxString &HlUrl) const;
	//! Modifica el nodo url del nodo textura de un elemento de terreno.
	void ModifyTextureNode(const wxXmlNode *pContextNode);
	/** Analiza si el Uri corresponde a ruta de sistema de archivos  */
	bool IsFileSystemPath(std::string NodeUri);

	wxXmlNode *pInfoNode_; ///< Nodo informacion del PAT.
	std::list<Context*> *pContextList_; ///< Lista de Contextos.
	bool isUserPat_; ///< indica si trabaja con PAT de Usr. o Propietario.
	bool isNew_; ///< indica si es PAT nuevo o esta editando uno ya creado.
	bool isEditing_; ///< Flag que indica si se esta editando (vs creando) el PAT (ver tck #153)
	wxString oldNamePat_; ///< para verificar si se cambio el nombre del PAT.
	bool updateMainToolData_; ///<indica cuando se debe actualizar la informacion del maintool
	suri::DataViewManager* pDataViewManager_; /** Contenedor de Managers */
	EVENT_OBJECT_PROPERTY(PatInfoPartEvent);

};

#endif /*PATINFOPART_H_*/
