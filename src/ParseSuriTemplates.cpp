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
