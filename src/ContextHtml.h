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
