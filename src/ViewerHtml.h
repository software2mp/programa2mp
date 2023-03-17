/* Copyright (c) 2006-2023 SpaceSUR and CONAE

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This file is part of the SuriLib project and its derived programs. You must retain
this copyright notice and license text in all copies or substantial
portions of the software.

If you develop a program based on this software, you must provide a visible
notice to its users that it contains code from the SuriLib project and provide
a copy of this license. The notice should be displayed in a way that is easily
accessible to users, such as in the program's "About" box or documentation.

For more information about SpaceSUR, visit <https://www.spacesur.com>.
For more information about CONAE, visit <http://www.conae.gov.ar/>. */

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
