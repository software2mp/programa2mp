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

#include "Context3D.h"

//Includes standard
#ifdef __DEBUG__
#include <iostream>
#endif

//Includes Suri
#include "suri/Viewer3D.h"
#include "suri/VectorElement.h"
#include "suri/TerrainElement.h"
#include "suri/LayerList.h"
#include "suri/xmlnames.h"
#include "suri/messages.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"

//Includes App
#include "messages.h"
#include "Context2D.h"	///< Por la logica de activacion de elementos
#include "MultipleRasterElement3DActivationLogic.h"
#include "SingleRasterElementActivationLogic.h"
#include "HotLinkVectorElement.h"

AUTO_REGISTER_CLASS(Context, Context3D, 1, wxWindow*)

/**
 * Constructor.
 * @param[in]	pParent: puntero a ventana padre.
 * @param[in] pDataViewManager puntero a visualizador del contexto.
 */
Context3D::Context3D(wxWindow *pParent, suri::DataViewManager* pDataViewManager) :
      Context(pParent, pDataViewManager, wxT(TYPE_PROPERTY_VALUE_TERRAIN)) //No se traduce, elemento de XML
{
   windowTitle_ = _(caption_TERRAIN_CONTEXT);
   typeContext_ = Context3DType;
   supportedFilesExt_ = wxString::Format("%s%s%s", filter_CONTEXT_TERRAIN_FILES_LOAD,
   filter_FORMAT_SEPARATOR,
                                         filter_ALL_FILES_LOAD).c_str();
}

/**
 * Destructor.
 */
Context3D::~Context3D() {
}

/**
 * Crea/carga la ventana del Contexto. A la ventana creada por la clase base de
 * los contextos le agrega el visor del contexto y las herramientas del mismo.
 */
bool Context3D::CreateToolWindow() {
   Context::CreateToolWindow();

   suri::Viewer3D *pviewer = new suri::Viewer3D(pToolWindow_);
   AddViewer(pviewer);
   pElementList_->SetActivationLogic(
         new suri::MultipleRasterElement3DActivationLogic());

   return true;
}

/**
 * Agrega un elemento a la lista del contexto.
 * Para poder crear los distintos tipos de elemento que admite el contexto,
 * intenta crear todos los tipos de elemntos hasta que alguno machee. Si no
 * machea ninguno retorna NULL.
 *
 * TerrainElement se crea en forma partida, primero el nombre del terreno
 *  y luego el de la textura (opcional en caso de TerrainElement, pero
 *  mandatorio en el Contexto).

 * Valida que no se inserte un elemento repetido.
 * @param[in] UrlElement: url del elemento a agregar.
 * @param[in] ManuallyAdd true indica elemento cargado por el usuario
 *            (a mano)
 * @param[in] ManuallyAdd false elemento cargado de forma automatica, al
 *            abrir un PAT.
 * @return puntero al elemento creado.
 * @return NULL en caso de elemento no valido para el contexto.
 * \attention: el puntero devuelto no debe ser eliminado ya que la lista de
 * elementos(LayerList) se hace cargo del mismo.
 */
suri::Element *Context3D::AddElement(const wxString& UrlElement,
                                     const bool &ManuallyAdd,
                                     const wxXmlNode* pElementNode) {
   // Verifico que el elemento no se encuentre en la lista
   if (!CanInsertElement(UrlElement)) return NULL;

   wxFileName filename(UrlElement);

   suri::Element* pelement = suri::Element::Create("HotLinkVectorElement",
                                                   UrlElement.c_str());
   if (!pelement) pelement = suri::VectorElement::Create(UrlElement.c_str());

   if (!pelement) {
      suri::Element* pttelement = suri::TerrainElement::Create(UrlElement.c_str());
      if (!pttelement) {
         // Puede que sea una textura
         pttelement = suri::RasterElement::Create(UrlElement.c_str());
         if (!pttelement) return NULL;
      }

      pelement = pttelement;
   }

   if (pelement) pelement->SetName(filename.GetName());

   const char *pdstypes[] = { "RasterDatasource", "VectorDatasource" };
   suri::DatasourceInterface* pdatasource = NULL;
   for (int i = 0; i < 2 && pdatasource == NULL; ++i) {
      pdatasource = suri::DatasourceInterface::Create(pdstypes[i], pelement);
   }

   if (pdatasource) {
      pDataViewManager_->GetDatasourceManager()->AddDatasourceWithNotification(
            pdatasource, viewcontextId_,
            static_cast<suri::ViewcontextInterface::ViewcontextType>(GetContextType()));
   }

   return pelement;

}

