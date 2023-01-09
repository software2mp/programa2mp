#include "ImageTagHandler.h"

//Includes standard

//Includes Suri

//Includes Wx

//Includes App

//Defines

//Forwards.

//--- TAGHANDLER ---
/**
 * Constructor.
 */
ImageTagHandler::ImageTagHandler() {
   pParser_ = NULL;
}

/**
 * Constructor de copia.
 * @param[in]	pImgParser: puntero a un objeto ImageParser.
 */
ImageTagHandler::ImageTagHandler(ImageParser *pImgParser) {
   pParser_ = pImgParser;
}

/**
 * Devuelve el tipo de tag a parsear.
 * @return wxString con el tipo de tag a parsear.
 */
wxString ImageTagHandler::GetSupportedTags() {
   return "IMG";
}

/**
 * Este metodo es el "kernel" de cada parseador. Es llamado por el parseador
 * cada vez que se detecta un tag soportado.
 * Si el tag contiene el parametro "SRC", agrega el url de la imagen al vector
 * de urls de imagenes del parseador.
 * @param[in]	Tag: tag html.
 * @return true si el tag contiene el parametro SRC. false C.O.C.
 */
bool ImageTagHandler::HandleTag(const wxHtmlTag &Tag) {
   if (Tag.HasParam(wxT("SRC"))) {
      wxString src(Tag.GetParam(wxT("SRC")));
      pParser_->AddString(src);
      ParseInner(Tag);
      return true;
   } else
      return false;
}
