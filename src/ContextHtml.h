//<COPYRIGHT> a reemplazar a futuro

#ifndef CONTEXTHTML_H_
#define CONTEXTHTML_H_

//Includes standard
#include <string>

//Includes Suri

//Includes Wx
#include "wx/xml/xml.h"

//Includes App
#include "Context.h"

//Defines

namespace suri {
class HtmlElement;
class MultimediaElement;
class DataViewManager;
class DatasourceInterface;
}

//! Clase concreta del contexto "HTML"
/**
 * Esta clase es utlizada para crear los contextos Documentos, Fotografias y
 * Actividades.
 * Estos contextos permiten cargar entre sus elementos (dependiendo del tipo
 *   pasado al constructor):
 * 	- Documentos, Actividades: solo elementos Html.
 * 	- Fotografias: solo imagenes
 */
class ContextHtml : public Context {
   //! Ctor. de Copia.
   ContextHtml(const ContextHtml &ContextHtml);

public:
   //! Ctor. default.
   ContextHtml(wxWindow *pParent = NULL, suri::DataViewManager* pDataViewManager = NULL,
		   ContextType Type = ContextDocumentsType);
   //! Dtor.
   virtual ~ContextHtml();
   //! Crea la ventana
   virtual bool CreateToolWindow();
protected:
   //! Clase derivada donde se crea y agrega el elemento del contexto
	virtual suri::Element *AddElement(const wxString& Filename,
			const bool &ManuallyAdd, const wxXmlNode* pElementNode = NULL);
   //! Agrega un elemento al contexto
   virtual suri::Element *AddElement(const wxXmlNode *pElement, const bool &ManuallyAdd = false);
private:
   suri::DataViewManager* pDataViewManager_; ///< Visualizador de contexto.
   /** Crea un element a partir de un url pasada por parametro **/
   suri::Element* CreateElementFromUrl(const std::string Url);
   /** Crea una fuente de datos a partir de un element **/
   suri::DatasourceInterface* CreateDatasourceFromElement(suri::Element* pElement);
};

#endif /*CONTEXTHTML_H_*/
