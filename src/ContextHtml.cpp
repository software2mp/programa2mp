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

#include "ContextHtml.h"

//Includes standard
#ifdef __DEBUG__
#include <iostream>
#endif

//Includes Suri
#include "suri/LayerList.h"
#include "suri/xmlnames.h"
#include "suri/ElementPropertiesWidget.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/imaglist.h"
#include "wx/listbook.h"
#include "wx/xrc/xmlres.h"

//Includes App
#include "HtmlElement.h"
#include "MultimediaElement.h"
#include "ViewerHtml.h"
#include "messages.h"

//Defines

//Forwards
// Forward para no tener que crear un .h
void InitCHtmlToolbarXmlResource();

/**
 * Constructor.
 * @param[in]	pParent: puntero a ventana padre.
 * @param[in]	Type: tipo de Contexto.
 * @param[in] pDataViewManager puntero a visualizador del contexto.
 */
ContextHtml::ContextHtml(wxWindow *pParent, suri::DataViewManager* pDataViewManager, ContextType Type) :
      Context(pParent, pDataViewManager, wxT(TYPE_PROPERTY_VALUE_HTML)) {
   windowTitle_ = _(caption_DOCUMENTS_CONTEXT);
   typeContext_ = Type;

   //inicializo los formatos con el default del contexto, si es de foto lo cambio.
   supportedFilesExt_ = filter_CONTEXT_DOCUMENT_FILES_LOAD;
   if (typeContext_ == ContextMultimediasType) supportedFilesExt_ =
         filter_CONTEXT_MULTIMEDIA_FILES_LOAD;

   supportedFilesExt_ =
         wxString::Format("%s%s%s", supportedFilesExt_.c_str(), filter_FORMAT_SEPARATOR,
                          filter_ALL_FILES_LOAD).c_str();
   pDataViewManager_ = pDataViewManager;
}

/**
 * Destructor
 */
ContextHtml::~ContextHtml() {
}

/**
 * Crea/carga la ventana del Contexto. A la ventana creada por la clase base de
 * los contextos le agrega el visor del contexto y las herramientas del Contexto
 * que corresponda.
 */
bool ContextHtml::CreateToolWindow() {
   Context::CreateToolWindow();

   AddViewer(new ViewerHtml(pToolWindow_, pElementList_));
   return true;
}

/**
 * Crea y agrega un elemento a la lista del contexto.
 * Para poder crear los distintos tipos de elemento que admite el contexto,
 * intenta crear todos los tipos de elemntos hasta que alguno machee. Si no
 * machea ninguno retorna NULL.
 * Valida que no se inserte un elemento repetido.
 * @param[in] UrlElement: url del elemento a agregar.
 * @param[in] ManuallyAdd true Indica el elemento esta siendo cargado por
 *  el usuario (a mano), en cuyo caso muestra la ventana de propiedades
 *  de fotografia para que el usuario complete el titulo y descripcion.
 * @param[in] ManuallyAdd false El elemento se carga de forma automatica
 *  (al abrir un PAT).
 * @return puntero al elemento creado
 * @return NULL en caso de error.
 * \attention: el puntero devuelto no debe ser eliminado ya que la lista de
 * elementos (LayerList) se hace cargo del mismo.
 */
suri::Element *ContextHtml::AddElement(const wxString& UrlElement,
		const bool &ManuallyAdd, const wxXmlNode* pElementNode) {
	//verifico que el elemento no se encuentre en la lista.
	if (!CanInsertElement(UrlElement))
		return NULL;

	wxFileName filename(UrlElement);
	suri::Element *pelement = NULL;

	if (typeContext_ == ContextDocumentsType
			|| typeContext_ == ContextActivitiesType)
		pelement = suri::HtmlElement::Create(UrlElement.c_str());
   if (!pelement && filename.GetExt().compare("pdf") == 0)
      pelement = suri::MultimediaElement::Create(UrlElement.c_str());
	if (typeContext_ == ContextMultimediasType)
		pelement = suri::MultimediaElement::Create(UrlElement.c_str());

	if (!pelement)
		return NULL;
	pelement->SetName(filename.GetName());
	if (pElementNode) {
		//obtengo el elemento de la lista y le reemplazo el nodo raiz.
		pelement->AddNode(NULL, new wxXmlNode(*pElementNode), true);
	}
	const char *pdstypes[] = { "HtmlDatasource" , "MultimediaDatasource"};
	suri::DatasourceInterface* pdatasource = NULL;
	for (int i = 0; i < 2 && pdatasource == NULL; ++i) {
		pdatasource = suri::DatasourceInterface::Create(pdstypes[i], pelement);
	}
	if (pdatasource) {
		pDataViewManager_->GetDatasourceManager()->AddDatasourceWithNotification(
				pdatasource, viewcontextId_, static_cast<suri::ViewcontextInterface::ViewcontextType>(GetContextType()));
	}
#ifdef __PHOTO_AUTOMATIC_PROPERTIES_LAUNCH__
	if (typeContext_ == ContextMultimediasType && ManuallyAdd) {
		//pprop se elimina al cerrar el showmodal
		suri::ElementPropertiesWidget *pprop =
				new suri::ElementPropertiesWidget(pDataViewManager_, pelement);
		wxCommandEvent event;
		pprop->OnButtonForward(event);
		pprop->ShowModal();
	}
#endif   // __PHOTO_AUTOMATIC_PROPERTIES_LAUNCH__
	return pelement;
}

/**
 * Objetivo: Agrega un elemento a la lista del contexto.
 * @param[in]	pNodeElement: nodo elemento del elemento a agregar al contexto.
 * @param[in]	ManuallyAdd: Se pasa como parametro al overload AddElement
 * @return devuelve el elemtno insertado.
 * @return NULL en caso de no poder agregar el elemento.
 */
suri::Element *ContextHtml::AddElement(const wxXmlNode *pNodeElement,
		const bool &ManuallyAdd) {
	//obtengo el path del archivo.
	wxXmlNode *pchild = pNodeElement->GetChildren();
	while (pchild && pchild->GetName().Cmp(URL_NODE) != 0)
		pchild = pchild->GetNext();

	//Genero el elemento de contexto y lo agrego a la lista.
	suri::Element *pelement = CreateElementFromUrl(pchild->GetNodeContent().c_str());
	if (!pelement)		//significa que el contexto no pudo interpretar el elemento.
		return NULL;
	//obtengo el elemento de la lista y le reemplazo el nodo raiz.
	pelement->AddNode(NULL, new wxXmlNode(*pNodeElement), true);
	suri::DatasourceInterface* pdatasource = CreateDatasourceFromElement(pelement);
	if (pdatasource)
		pDataViewManager_->GetDatasourceManager()->AddDatasourceWithNotification(
				pdatasource, viewcontextId_);
	return pelement;
}

/** Crea un element a partir de un url pasada por parametro **/
suri::Element* ContextHtml::CreateElementFromUrl(const std::string Url) {
	suri::Element *pelement = NULL;
	if (typeContext_ == ContextDocumentsType
			|| typeContext_ == ContextActivitiesType)
		pelement = suri::HtmlElement::Create(Url);

	if (typeContext_ == ContextMultimediasType)
		pelement = suri::MultimediaElement::Create(Url);
	return pelement;
}

/** Crea una fuente de datos a partir de un element **/
suri::DatasourceInterface* ContextHtml::CreateDatasourceFromElement(suri::Element* pElement) {
	const char *pdstypes[] = { "MultimediaDatasource", "HtmlDatasource" };
	suri::DatasourceInterface* pdatasource = NULL;
	for (int i = 0; i < 2 && pdatasource == NULL; ++i) {
		pdatasource = suri::DatasourceInterface::Create(pdstypes[i], pElement);
	}
	return pdatasource;
}
