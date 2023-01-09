//<COPYRIGHT> a reemplazar a futuro

#ifndef IMAGEPARSER_H_
#define IMAGEPARSER_H_

//Includes standard
#include <map>

//Includes Suri

//Includes Wx
#include <wx/wxprec.h>
#include <wx/html/htmlpars.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Includes App

//Defines

//Forwards

//! Clase soporte para poder parsear un html
/**
 * Esta Clase permite parsear un fuente html, y obtener todos los url de las
 * imagenes que contiene.
 */
class ImageParser : public wxHtmlParser {
   wxSortedArrayString imgArray_;  ///< vector con los url de las imagenes del html.
   wxString htmlSrc_;  ///< fuente html a parsear.
public:
   //! Ctor. default.
   ImageParser(const wxString &HtmlSrc);
   //! Agrega un url de imagen al vector.
   void AddString(wxString Txt);
   //! Devuelve el producto del parseo del html.
   wxObject* GetProduct() {
      return NULL;
   }
   //! (Ver doc. wx)
   void AddText(const wxChar* pTxt);
   //! Devuelve los url de las imagenes.
   wxArrayString GetSources();
};

#endif /*IMAGEPARSER_H_*/
