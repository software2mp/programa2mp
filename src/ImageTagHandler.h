//<COPYRIGHT> a reemplazar a futuro

#ifndef IMAGETAGHANDLER_H_
#define IMAGETAGHANDLER_H_

//Includes standard
#include <map>

//Includes Suri

//Includes Wx
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Includes App
#include "ImageParser.h"

//Defines

//Forwards

//! Clase soporte para ImageParser.
/**
 * Esta clase permite obtener los url de las imagenes en un html.
 */
class ImageTagHandler : public wxHtmlTagHandler {
   ImageParser *pParser_;  ///< parseador.
public:
   //! Ctor.
   ImageTagHandler();
   //! Ctor. de copia.
   ImageTagHandler(ImageParser *pImgParser);
   //! Devuelve el tipo de tag a parsear.
   wxString GetSupportedTags();
   //! Parsea un tag html.
   bool HandleTag(const wxHtmlTag &Tag);
};

#endif /*IMGTAGHANDLER_H_*/
