//<COPYRIGHT> a reemplazar a futuro

#include "ImageParser.h"

//Includes standard

//Includes Suri

//Includes Wx

//Includes App
#include "ImageTagHandler.h"

//Defines

//Forwards

//--- IMGPARSER ---
ImageParser::ImageParser(const wxString &Url) :
      htmlSrc_(Url) {
   AddTagHandler(new ImageTagHandler(this));
}

/**
 * Agrega un item al vector de urls de imagenes.
 * @param[in]	Txt: url de imagen a agregar al vector de urls
 */
void ImageParser::AddString(wxString Txt) {
   if (imgArray_.Index(Txt) == wxNOT_FOUND)  //Para que inserte sin repeticion.
      imgArray_.Add(Txt);
}

/**
 * Este metodo debe ser implementado ya que es llamado por DoParsing.
 */
void ImageParser::AddText(const wxChar *pTxt) {
}

/**
 * Parsea el html obteniendo los url de las imagenes que lo componen.
 * @return vector con los url de las imagenes del html
 */
wxArrayString ImageParser::GetSources() {
   Parse(htmlSrc_);
   return imgArray_;
}
