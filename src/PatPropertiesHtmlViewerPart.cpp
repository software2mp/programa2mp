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

#include "PatPropertiesHtmlViewerPart.h"

//Includes standard

//Includes Suri
#include "suri/Configuration.h"

//Includes Wx

//Includes App
#include "PatFile.h"
#include "ParseSuriTemplates.h"
#include "messages.h"

//Defines
#define HTML_ERROR_HEAD "<html><head><title></title></head><body bgcolor=\"rgb(150, 150, 150)\"><font face=\"Lucida Grande, Lucida Sans Unicode\"><table align=\"center\"><tr><td><table width=\"460\" bgcolor=\"rgb(255, 255, 255)\" cellspacing=\"0\" cellpadding=\"0\"><tr><td><table bgcolor=\"rgb(114, 175, 224)\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\"90%\" valign=\"center\"><font size=\"5\" color=\"rgb(255, 255, 255)\"><em>Producto Autocontenido Tem&aacute;tico</em></font></td><td width=\"10%\" valign=\"center\" align=\"right\"></tr></table><table cellpadding=\"10\"><tr><td><table cellpadding=\"0\"><tr><td><tr><td colspan=\"2\" align=\"center\"><h1><font color=\"rgb(4, 26, 204)\">"
#define HTML_ERROR_TAIL "</font></h1></td></tr></td></tr></table></td></tr></table></td></tr></table></td></tr></table></body></html>"

#define suritag_TITLE "<suri src=\"titulo\"/>"

#define filename_PAT_TEMPLATE "pat.html"
#define filename_PAT_EMPTY_TEMPLATE "pat_empty.html"

//Forwards

/**
 * Constructor.
 * @param[in] PatPath: path del archivo descripcion.xml
 */
PatPropertiesHtmlViewerPart::PatPropertiesHtmlViewerPart(const std::string &PatPath) :
      patPath_(PatPath) {
   windowTitle_ = _(caption_PAT_PROPERTIES);
   modified_ = false;
}

/**
 * Destructor.
 */
PatPropertiesHtmlViewerPart::~PatPropertiesHtmlViewerPart() {

}

//! Se crea la ventana de herramienta en las clases derivadas
/**
 * Carga la ventana diseniada. (Ver HtmlViewerPart::CreateToolWindow())
 * @return true en caso de poder cargar la ventana. false C.O.C.
 */
bool PatPropertiesHtmlViewerPart::CreateToolWindow() {
   return suri::HtmlViewerPart::CreateToolWindow();
}

/**
 * Inicializa los controles.
 * Obtiene el template para generar el html, genera el html y lo carga.
 * En caso de no existir el template u otro error, carga un html con leyenda de
 * error.
 */
void PatPropertiesHtmlViewerPart::SetInitialValues() {
   std::string pathdirhtml = suri::Configuration::GetParameter("pat_html_dir",
                                                               wxT("./"));
   wxString html = wxT("");

   //Verifica que existan los templates.
   if (!CheckDataDirectory(pathdirhtml)) {
      html = HTML_ERROR_HEAD + wxString(_(message_MISSING_TEMPLATE))
            + wxString(HTML_ERROR_TAIL);
      LoadHtml(html.c_str());
      return;
   }

   //obtengo el path del template y un mapa con los parametros a escribir al template.
   if (!GetTemplateHtml(pathdirhtml, html)) {
      LoadHtml(html.c_str());
      return;
   }

   //el switcheo entre CWD es para que el html localize las imagenes.
   wxString cwd = wxGetCwd();
   wxSetWorkingDirectory(pathdirhtml);
   LoadHtml(html.c_str());
   wxSetWorkingDirectory(cwd);
}

/**
 * Guarda el path del archivo descripcion.xml(de donde se obtiene el nodo
 * informacion) y carga el html.
 * @param[in]	PatPath: path del archivo descripcion.xml
 */
void PatPropertiesHtmlViewerPart::SetHtml(const std::string &PatPath) {
   patPath_ = PatPath;
   SetInitialValues();
}

/**
 * Verifica que exista el directorio donde se encuentran los templates html para
 * visualizar la informacion del PAT.
 * \post Crea los directorios en caso de no existir.
 *
 * @param[in]	Path: path del directorio a verificar.
 * @return si existe o si puede crearlo retorna true; otro caso false.
 */
bool PatPropertiesHtmlViewerPart::CheckDataDirectory(const wxString &Path) {
   wxFileName fn(Path);

   if (!fn.Normalize() || !fn.IsDir())
      REPORT_AND_FAIL_VALUE("D:%s no es un directorio valido.", false,
                            fn.GetFullPath().c_str());
   if (!fn.DirExists())
      if (!fn.Mkdir(fn.GetFullPath().c_str(), 0755, wxPATH_MKDIR_FULL))
         REPORT_AND_FAIL_VALUE( "D:Error al crear directorio %s.", false,
                               fn.GetFullPath().c_str());
   //Busco que existan los templates html.
   if (!wxFileName::FileExists(Path + filename_PAT_TEMPLATE)
         || !wxFileName::FileExists(Path + filename_PAT_EMPTY_TEMPLATE)) {
      SHOW_ERROR(message_MISSING_TEMPLATE);
      //no se traduce, lo hace SHOW_
      return false;
   }
   return true;
}

/**
 * Genera el codigo html que se va a mostrar por pantalla en un string.
 *
 * @param[in]	PathDirHtml: ruta del directorio del template.
 * @param[out]	Html: codigo html del html a mostrar.
 * \post Para el caso de retorno false, se genera un HTML de error
 * @return true si se genera el HTML con la informacion.
 * @return false en caso de error
 */
bool PatPropertiesHtmlViewerPart::GetTemplateHtml(const wxString &PathDirHtml,
                                                  wxString &Html) {
   PatFile pat(patPath_);
   ParseSuriTemplates *pparse;
   std::map<std::string, std::string> params;
   wxString pathhtml = PathDirHtml;

   //obtengo los parametros para reemplazar en el html.
   if (pat.GetPatType() != PatFile::PatInvalid) {
      pathhtml += filename_PAT_TEMPLATE;
      pparse = new ParseSuriTemplates(pathhtml);
      pat.GetPatDescription(params);
   } else {
      pathhtml += filename_PAT_EMPTY_TEMPLATE;
      pparse = new ParseSuriTemplates(pathhtml);
      params.insert(std::make_pair(suritag_TITLE, _(message_SELECT_PAT)));
   }

   //obtengo en html el nuevo html a mostrar.
   if (!pparse->GetNewHtml(params, Html)) {
      Html = HTML_ERROR_HEAD + wxString(_(message_MISSING_TEMPLATE))
            + wxString(HTML_ERROR_TAIL);
      delete pparse;
      return false;
   }

   delete pparse;
   return true;
}
