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

