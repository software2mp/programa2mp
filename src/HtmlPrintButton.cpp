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

#include "HtmlPrintButton.h"

//Includes standard

//Includes Suri
#include "suri/Element.h"
#include "suri/xmlnames.h"

//Includes Wx

//Includes App
#include "HtmlElement.h"
#include "MultimediaElement.h"
#include "ApplicationWindowManager.h"
#include "resources.h"
#include "messages.h"

//Defines
#define DIR_DATA "data/fotografia/"

//! Ctor. default.
HtmlPrintButton::HtmlPrintButton(wxWindow *pViewerToolbar) :
      suri::Button(pViewerToolbar, wxT(button_PRINT_DOCUMENT), _(caption_PRINT)) {
}

/**
 * Recorre la lista de elementos, y para cada elemento activo, dependiendo del
 * tipo de elemento despliega el dialogo de vista previa, desde el cual se puede
 * imprimir el elemento.
 * \pre el elemento debe ser del tipo Fotografia o Html.
 * @param[in]	Event evento de click. (No realiza alguna accion en particular).
 */
void HtmlPrintButton::OnButtonClicked(wxCommandEvent &Event) {
   suri::Element *pelement = NULL;

   //Recorro la lista de elementos buscando los activos.
   int listcount = pList_->GetElementCount(true);
   for (int i = 0; i < listcount; i++)
      if (pList_->GetElement(i, true)->IsActive()) {
         pelement = pList_->GetElement(i, true);
         // muestro dialogo de vista previa segun tipo de elemento.
         suri::MultimediaElement *pphotoelement = dynamic_cast<suri::MultimediaElement*>(pelement);
         if (pphotoelement) {
            wxString html = wxT("");
            pphotoelement->GetHtmlInfo(html);
            ApplicationWindowManager::Instance().GetPrinter()->PreviewText(
                  html, wxString(DIR_DATA));
         } else if (dynamic_cast<suri::HtmlElement*>(pelement))
            ApplicationWindowManager::Instance().GetPrinter()->PreviewFile(
                  pelement->GetNode(URL_NODE)->GetNodeContent());
      }
}

