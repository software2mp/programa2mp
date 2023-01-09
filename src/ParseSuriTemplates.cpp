//<COPYRIGHT> a reemplazar a futuro

#include "ParseSuriTemplates.h"

//Includes standard

//Includes Suri
#include "logmacros.h"

//Includes Wx
#include "wx/ffile.h"
#include "wx/filename.h"

//Includes App

//Defines

//Forwards

//--- PARSESURITEMPLATES ---
/**
 * Constructor.
 * @param[in]	TemplatePath: path del template a utilizar
 */
ParseSuriTemplates::ParseSuriTemplates(const wxString &TemplatePath) :
      TemplatePath_(TemplatePath) {

}

//! dtor.
ParseSuriTemplates::~ParseSuriTemplates() {
}

/**
 * Devuelve un Html con los tag reemplazados por los parametros recibidos.
 * @param[in]	Params: mapa con los tags del template(clave), tag para reemplazar(valor).
 * @param[out]	NewHtml: codigo html con los valores reemplazados.
 * @return true si pudo hacer reemplazo. false C.O.C.
 */
bool ParseSuriTemplates::GetNewHtml(std::map<std::string, std::string> Params,
wxString &NewHtml) {
   NewHtml.Empty();
   if (TemplatePath_.IsEmpty() || Params.empty()) return false;

   wxFFile file(TemplatePath_);
   if (!file.IsOpened()) return false;

   if (!file.ReadAll(&NewHtml))  //leo todo el contenido del template
      return false;

   std::map<std::string, std::string>::iterator it = Params.begin();
   for (; it != Params.end(); it++)
      if ((NewHtml.Replace(it->first.c_str(), it->second.c_str(), false)) == 0)
      REPORT_DEBUG("D:No se encuentra tag: %s", it->first.c_str());

   return true;
}

/**
 * Objetivo: Genera un archivo html en el directorio temporal del sistema
 * con el contenido de HtmlSrc
 * @param[in]	HtmlSrc: contenido del archivo a generar.
 * @return Ok? path del archivo salvado : cadena vacia.
 */
wxString ParseSuriTemplates::SaveHtml(const wxString &HtmlSrc) {
   if (HtmlSrc.IsEmpty()) return wxT("");

   //genero archivo temporal
   wxString pathtemp = wxFileName::GetTempDir();
   pathtemp += wxFileName::GetPathSeparator();
   pathtemp += "sur.html";
//	wxString auxpath = wxFileName::CreateTempFileName(pathtemp).c_str();
//	if(auxpath.IsEmpty()) return wxT("");
   wxString auxpath = pathtemp;
   //
   wxFFile file(auxpath, "w");
   if (!file.IsOpened()) return wxT("");
   if (!file.Write(HtmlSrc)) return wxT("");

   return auxpath;
}
