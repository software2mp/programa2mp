//<COPYRIGHT> a reemplazar a futuro

#ifndef PARSESURITEMPLATES_H_
#define PARSESURITEMPLATES_H_

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

//! Clase para parsear y modificar templates HTML
/**
 * Esta clase permite parsear un html, reemplazando ciertos tags por otros
 * recibidos por parametro.
 */
class ParseSuriTemplates {
public:
   //! ctor.
   ParseSuriTemplates(const wxString &TemplatePath);
   //! dtor.
   ~ParseSuriTemplates();
   //! Devuelve un Html con los tag reemplazados por los parametros recibidos. 
   bool GetNewHtml(std::map<std::string, std::string> Params,
   wxString &NewHtml);
   //! Baja el Html a disco
   wxString SaveHtml(const wxString &HtmlSrc);

private:
   wxString TemplatePath_;  ///< path del template a utilizar
};

#endif /*PARSESURITEMPLATES_H_*/
