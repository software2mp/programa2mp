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

#include "PhotoPropertiesPart.h"

//Includes standard

//Includes Suri
#include "suri/xmlnames.h"

//Includes Wx
#include "wx/xrc/xmlres.h"

//Includes App
#include "messages.h"
#include "resources.h"

//Defines

//Forwards

START_IMPLEMENT_EVENT_OBJECT(PhotoPropertiesPartEvent, PhotoPropertiesPart)IMPLEMENT_EVENT_CALLBACK(OnUIUpdate, OnUIUpdate(event), wxUpdateUIEvent)
END_IMPLEMENT_EVENT_OBJECT

/**
 * @param[in]	pElement: puntero al Elemento.
 */
PhotoPropertiesPart::PhotoPropertiesPart(PhotoElement *pElement) :
      pElement_(pElement), pTextTitle_(NULL), pTextDescription_(NULL),
      NEW_EVENT_OBJECT(PhotoPropertiesPartEvent)
{
   windowTitle_ = _(caption_PHOTO_PROPERTIES);
}

/**
 * Destructor.
 */
PhotoPropertiesPart::~PhotoPropertiesPart() {
   DELETE_EVENT_OBJECT
   ;
}

/**
 * Carga la ventana diseniada. En caso de exito.
 * \post asocia evento UIUpdate a las textctrl para poder verificar su estado.
 * @return true en caso de poder cargar la ventana, false C.O.C.
 */
bool PhotoPropertiesPart::CreateToolWindow() {
   pToolWindow_ = wxXmlResource::Get()->LoadPanel(pParentWindow_,
                                                  wxT("ID_PHOTO_PROPERTIES"));
   if (!pToolWindow_) return false;

   pTextTitle_ = XRCCTRL(*pToolWindow_, wxT("ID_PHOTO_TITLE"), wxTextCtrl);
   pTextDescription_ = XRCCTRL(*pToolWindow_,wxT("ID_PHOTO_DESCRIPTION"),
         wxTextCtrl);
   if (pTextTitle_)
      pTextTitle_->Connect(wxEVT_UPDATE_UI,
                           wxUpdateUIEventHandler(PhotoPropertiesPartEvent::OnUIUpdate),
                           NULL, pEventHandler_);
   if (pTextDescription_)
      pTextDescription_->Connect(
            wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(PhotoPropertiesPartEvent::OnUIUpdate), NULL,
            pEventHandler_);
   return true;
}

/**
 * Inicializa los controles.
 * Carga en las textctrl el contenido del nodo caracteristicas.
 */
void PhotoPropertiesPart::SetInitialValues() {
   pTextTitle_->SetValue(pElement_->GetTitle());
   pTextDescription_->SetValue(pElement_->GetText());
}

//! Devuelve el estado del Part.
bool PhotoPropertiesPart::IsEnabled() {
   return true;
}

/**
 * Verifica el estado de las textctrl y si su contenido es distinto al contenido
 * del nodo caracteristicas.
 * @return true en caso de que haya cambios para salvar. false C.O.C.
 */
bool PhotoPropertiesPart::HasChanged() {
   wxString value = wxT("");
   value = pTextTitle_->GetValue();
   if (value.Cmp(pElement_->GetTitle()) != 0 && modified_) return true;

   value = pTextDescription_->GetValue();
   if (value.Cmp(pElement_->GetText()) != 0 && modified_) return true;
   return false;
}

/**
 * Guarda en el nodo caracteristicas el contenido de las textctrl.
 * @return true.
 */
bool PhotoPropertiesPart::CommitChanges() {
   pElement_->SetTitle(pTextTitle_->GetValue());
   pElement_->SetText(pTextDescription_->GetValue());
   pElement_->SetChanged();
   pElement_->SendViewerUpdate();
   modified_ = false;
   pTextTitle_->DiscardEdits();
   pTextDescription_->DiscardEdits();
   return true;
}

/**
 * Restaura los ultimos valores guardados en los controles del Part.
 */
bool PhotoPropertiesPart::RollbackChanges() {
   return true;
}

/**
 * Este metodo se ejecuta cuando se dispara el evento UIUpdate.
 * Tiene por objetivo, determinar si las textctrl cambiaron su contenido.
 * \todo El evento UIupdate debe actualizar los objetos de UI y no determinar
 *      un estado, esto se debe hacer en el evento correspondiente de los
 *      wxTextCtrls o con Validators o con TransferDataTo/FromWindow.
 * @param[in]	Event: evento.
 */
void PhotoPropertiesPart::OnUIUpdate(wxUpdateUIEvent &Event) {
   modified_ = (pTextTitle_->IsModified() || pTextDescription_->IsModified());
   Event.SetUpdateInterval(100);
   Event.Skip();
}

/**
 * Objetivo: Devuelve el icono de propiedades
 * Por ahora no tiene en cuenta el X e Y dado que devuelve el de 32x32 pixels
 */
void PhotoPropertiesPart::GetWindowIcon(wxBitmap& ToolBitmap, int X, int Y) const {
   GET_BITMAP_RESOURCE(icon_ELEMENT_PHOTO_PROPERTIES, ToolBitmap);
}

//----------------------------Fin de PhotoPropertiesPart----------------------------
