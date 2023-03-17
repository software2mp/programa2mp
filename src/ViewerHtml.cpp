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

#include "ViewerHtml.h"

//Includes standard

//Includes Suri
#include "suri/xmlnames.h"
#include "suri/Configuration.h"
#include "suri/HtmlFileRepresentationGetter.h"
#include "suri/MultimediaHtmlRepresentationGetter.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"
#include "wx/html/htmlwin.h"
#include "wx/mimetype.h"

//Includes App
#include "messages.h"
#include "HtmlElement.h"
#include "HtmlPrintButton.h"

//Defines
#define HTML_ERROR_HEAD "<html><head><title></title></head><body bgcolor=\"rgb(150, 150, 150)\"><font face=\"Lucida Grande, Lucida Sans Unicode\"><table align=\"center\"><tr><td><table width=\"460\" bgcolor=\"rgb(255, 255, 255)\" cellspacing=\"0\" cellpadding=\"0\"><tr><td><table bgcolor=\"rgb(114, 175, 224)\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\"90%\" valign=\"center\"><font size=\"5\" color=\"rgb(255, 255, 255)\"><em>Fotograf&iacute;a</em></font></td><td width=\"10%\" valign=\"center\" align=\"right\"></tr></table><table cellpadding=\"10\"><tr><td><table cellpadding=\"0\"><tr><td><tr><td colspan=\"2\" align=\"center\"><h1><font color=\"rgb(4, 26, 204)\">"
#define HTML_ERROR_TAIL "</font></h1></td></tr></td></tr></table></td></tr></table></td></tr></table></td></tr></table></body></html>"

START_IMPLEMENT_EVENT_OBJECT(ViewerHtmlEventHandler, ViewerHtml)
   IMPLEMENT_EVENT_CALLBACK(OnLinkClicked, OnLinkClicked(event), wxHtmlLinkEvent)
END_IMPLEMENT_EVENT_OBJECT

//Forwards
// Forward para no tener que crear un .h
void InitHtmlViewerXmlResource();

/**
 * Constructor.
 * @param[in]	pParent: puntero a la ventana padre.
 * @param[in]	pLayerList: lista de elementos.
 */
ViewerHtml::ViewerHtml(wxWindow *pParent, suri::LayerList *pLayerList) :
      ViewerWidget(pParent), pLayerList_(pLayerList),
      pHtmlGetter_(new suri::HtmlFileRepresentationGetter),
      pHtmlEventHandler_(new ViewerHtmlEventHandler(this)) {
   windowTitle_ = _(caption_HTML_VIEWER);
   // ancho del html de los pats
   wantedWidth_ = 495;
   pHtmlGetter_->SetSuccessor(new suri::MultimediaHtmlRepresentationGetter);
   CreateTool();
}

/**
 * Destructor.
 */
ViewerHtml::~ViewerHtml() {
}

/**
 * Carga la ventana diseniada.
 * @return true si pudo crear la ventana. false C.O.C.
 */
bool ViewerHtml::CreateToolWindow() {
   pToolWindow_ = wxXmlResource::Get()->LoadPanel(pParentWindow_, wxT("ID_HTMLVIEWER"));
   // llamo para que arme la ventana en caso de ser autonoma
   ViewerWidget::CreateToolWindow();
   XRCCTRL(*pToolWindow_,wxT("ID_HTMLRENDERER"),wxHtmlWindow)->Connect(
            wxEVT_COMMAND_HTML_LINK_CLICKED,
            wxHtmlLinkEventHandler(ViewerHtmlEventHandler::OnLinkClicked), NULL,
            pHtmlEventHandler_);
;
   return pToolWindow_ != NULL;
}

/**
 * Resetea el estado del visualizador. Setea una pagina en blanco.
 */
void ViewerHtml::ResetViewer() {
   XRCCTRL(*pToolWindow_,wxT("ID_HTMLRENDERER"),wxHtmlWindow)->SetPage(wxT(""));
}

//! Metodo que despliega un Elemento Html
/**
 * Obtiene el contenido del nodo url del elemento y carga dicho archivo.
 * @param[in]	pElement: puntero al elemento.
 */
void ViewerHtml::DisplayHtml(suri::HtmlElement *pElement) {
   std::string filename;
   //No se traduce, elemento de XML
   filename = pElement->GetNode(URL_NODE)->GetNodeContent();
   wxFileName file(filename);
   XRCCTRL(*pToolWindow_,wxT("ID_HTMLRENDERER"),wxHtmlWindow)->LoadFile(file);
}

//! Metodo que despliega un Elemento Multimedia
/**
 * Obtiene el html de la foto y lo despliega. Si hubo algun tipo de error,
 * muestra un html con texto de error.
 * @param[in]	pElement: puntero al elemento.
 */
void ViewerHtml::DisplayMultimedia(suri::MultimediaElement *pElement) {
   wxString html = wxT("");
   std::string htmlphotopath = 
         suri::Configuration::GetParameter(wxT("app_data_dir"), wxT("./")) + "fotografia/";
   if (CheckDataDirectory(htmlphotopath) && pElement->GetHtmlInfo(html)) {
      wxString cwd = wxGetCwd();
      wxSetWorkingDirectory(htmlphotopath);
      XRCCTRL(*pToolWindow_, wxT("ID_HTMLRENDERER"), wxHtmlWindow)->SetPage(html);
      wxSetWorkingDirectory(cwd);
   } else {
      html = wxString::Format("%s%s%s", HTML_ERROR_HEAD,
                              _(message_HTML_FILES_NOT_FOUND), HTML_ERROR_TAIL);
      XRCCTRL(*pToolWindow_, wxT("ID_HTMLRENDERER"), wxHtmlWindow)->SetPage(html);
   }
}

/**
 * Devuelve toolbar de herramientas del contexto.
 * @param[in]	pParent: puntero a la ventana padre.
 * @return	puntero a un wxWindow
 */
wxWindow *ViewerHtml::DoGetViewerTools(wxWindow *pParent) {
   wxWindow* ptoolbar = GetToolbar();
   //Si existe el toolbar como propiedad lo devuelve
   if (ptoolbar) return ptoolbar;

   //Sino lo arma y lo devuelve al final
   ptoolbar = new wxPanel(pParent, wxID_ANY);
   ptoolbar->SetSizer(new wxBoxSizer(wxHORIZONTAL));
   InitializeToolbar(ptoolbar, this, GetList());

   // Agrego boton de impresion de documentos.
   AddButtonTool(new HtmlPrintButton(ptoolbar), ViewerWidget::First,
                 ViewerWidget::StandAlone);
   return ptoolbar;
}

//! Despliega un elemento (puede ser funcion de RenderizationControl)
/**
 * Despliega un elemento, dependiendo de su tipo elige el metodo para desplegarlo.
 * @param[in]	pElement: puntero al elemento.
 */
void ViewerHtml::DisplayElement(suri::Element *pElement) {
   std::string html;
   if (!pHtmlGetter_->GetRepresentation(pElement, html))
      html = wxString::Format("%s%s%s", HTML_ERROR_HEAD,
                              _(message_HTML_FILES_NOT_FOUND), HTML_ERROR_TAIL);
   if (!wxFileName::FileExists(html) ||
            !XRCCTRL(*pToolWindow_,wxT("ID_HTMLRENDERER"),wxHtmlWindow)->LoadFile(wxFileName(html)))
      XRCCTRL(*pToolWindow_, wxT("ID_HTMLRENDERER"), wxHtmlWindow)->SetPage(html);
}

//! Oculta un elemento (puede ser funcion de RenderizationControl)
void ViewerHtml::HideElement(suri::Element *pElement) {
   //COMENTADO PORQUE AL AGREGAR UNA NUEVA FOTO, DADO QUE CAMBIAN LAS PROPIEDADES
   //SE EJECUTABA UN SENDUPDATE Y OCULTABA EL ELEMENTO DESPLEGADO
   //HtmlMultimediaPage_="";
   ResetViewer();
}

//! El elemento ha sido modificado
/**
 * Vuelve a desplegar el elemento.
 * @param[in]	pElement: puntero al elemento.
 */
void ViewerHtml::UpdateElement(suri::Element *pElement) {
   DisplayElement(pElement);
}

/**
 * Objetivo: Verifica que exista el directorio donde se encuentran los templates
 * html para visualizar la informacion del PAT.
 * En caso de que el directorio no exista lo crea y extrae los templates.
 * @param[in]	Path: path del directorio a verificar.
 * @return si existe o si puede crearlo retorna true; otro caso false.
 */
bool ViewerHtml::CheckDataDirectory(const wxString &Path) {
   wxFileName fn(Path);

   if (!fn.Normalize())
   REPORT_AND_FAIL_VALUE("D:Falla normalizacion", false);
   if (!fn.IsDir())
      REPORT_AND_FAIL_VALUE("D:%s no es un directorio valido", false,
                            fn.GetFullPath().c_str());
   if (!fn.DirExists())
   //Trata de crear el directorio. Si ya existe no hace nada.
      if (!fn.Mkdir(fn.GetFullPath().c_str(), 0755, wxPATH_MKDIR_FULL))
      REPORT_AND_FAIL_VALUE("D:Error al intentar crear directorio de templates", false);
   //Verifico que exista el template html.
   if (!wxFileName::FileExists(Path + "imagen.html")) {
      SHOW_ERROR(message_HTML_FILES_NOT_FOUND);
      return false;
   }
   return true;
}

/** MEtodo que se llama cuando se hace click sobre un link en el html **/
void ViewerHtml::OnLinkClicked(wxHtmlLinkEvent& Event) {
   wxString linkstr = Event.GetLinkInfo().GetHref();
   if (!linkstr.StartsWith("http:") && !linkstr.StartsWith("www.")) {
      wxFileName fname(linkstr);
      wxString hotlinkextention = fname.GetExt();
      wxFileType *photlinkmimetype = wxTheMimeTypesManager->GetFileTypeFromExtension(hotlinkextention);
      wxString commandstr = photlinkmimetype->GetOpenCommand("\"" + fname.GetFullPath() + "\"");
      wxExecute(commandstr);   
   } else {
      wxLaunchDefaultBrowser(linkstr);
   }
   // Freno la propagacion del evento para que no sea capturado por otro handler.
   Event.Skip(false);
}

