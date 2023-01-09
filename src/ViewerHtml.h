//<COPYRIGHT> a reemplazar a futuro

#ifndef HTMLVIEWER_H_
#define HTMLVIEWER_H_

//Includes standard
#include <set>

//Includes Suri
#include "suri/ViewerWidget.h"
#include "suri/LayerList.h"

//Includes Wx

//Includes App
#include "MultimediaElement.h"
#include "HtmlElement.h"
//Defines

//forwards
class wxHtmlLinkEvent;
class ViewerHtmlEventHandler;

namespace suri {
   class ElementHtmlRepresentationGetter;
}

//! Clase/herramienta que permite mostrar elementos Html y Fotografias.
/**
 * Esta Clase crea el entorno para trabajar con elementos Html y Fotografia.
 * Permite visualizar dichos elementos
 */
class ViewerHtml : public suri::ViewerWidget {
   //! Ctor. de Copia.
   ViewerHtml(const ViewerHtml &ViewerHtml);

public:
   //! Ctor. default.
   ViewerHtml(wxWindow *pParent, suri::LayerList *pLayerList);
   //! Dtor.
   virtual ~ViewerHtml();
   //! Creacion de la ventana
   virtual bool CreateToolWindow();
   //! Resetea el estado del visualizador
   virtual void ResetViewer();
   //! Metodo que despliega un Elemento Html
   void DisplayHtml(suri::HtmlElement *pElement);
   //! Metodo que despliega un Elemento Multimedia
   void DisplayMultimedia(suri::MultimediaElement *pElement);

protected:
   /** MEtodo que se llama cuando se hace click sobre un link en el html **/
   void OnLinkClicked(wxHtmlLinkEvent& Event);
   //! Permite obtener la ventana con las herramientas del viewer
   virtual wxWindow *DoGetViewerTools(wxWindow *pParent);
   //! Despliega un elemento (puede ser funcion de RenderizationControl)
   virtual void DisplayElement(suri::Element *pElement);
   //! Oculta un elemento (puede ser funcion de RenderizationControl)
   virtual void HideElement(suri::Element *pElement);
   //! El elemento ha sido modificado
   virtual void UpdateElement(suri::Element *pElement);

private:
   friend class ViewerHtmlEventHandler;
   bool CheckDataDirectory(const wxString &Path = wxT(""));
   suri::LayerList *pLayerList_;  ///< Lista de elementos del contexto.
   suri::ElementHtmlRepresentationGetter* pHtmlGetter_;
   ViewerHtmlEventHandler* pHtmlEventHandler_;
};

#endif /*HTMLVIEWER_H_*/
