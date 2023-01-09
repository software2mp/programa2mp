//<COPYRIGHT> a reemplazar a futuro

#ifndef HTMLPRINTBUTTON_H_
#define HTMLPRINTBUTTON_H_

//Includes standard

//Includes Suri
#include "suri/Button.h"

//Includes Wx

//Includes App

//Defines

/**
 * Esta clase representa un boton que provee de funcionalidad para imprimir
 * Elementos Html.
 */

class HtmlPrintButton : public suri::Button {
   //! Ctor. de Copia.
   HtmlPrintButton(const HtmlPrintButton &Button);
public:
   //! Ctor. default.
   HtmlPrintButton(wxWindow *pViewerToolbar);
   //! Dtor.
   ~HtmlPrintButton() {
   }
   ;
   //! Evento de Click sobre el boton.
   virtual void OnButtonClicked(wxCommandEvent &Event);
};

#endif /* HTMLPRINTBUTTON_H_ */
