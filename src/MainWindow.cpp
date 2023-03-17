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

//Includes standard
#include <string>
#include <list>
#ifdef __DEBUG__
#include <iostream>
#endif

//Includes Suri
#include "suri/ProgressManager.h"
#include "suri/PartContainerWidget.h"
#include "suri/Configuration.h"
#include "suri/xmlnames.h"
#include "suri/FileManagementFunctions.h"
#include "wxmacros.h"
#include "suri/DataViewManager.h"
#include "suri/LibraryManager.h"
#include "suri/LibraryManagerFactory.h"
#include "DefaultDatasourceManager.h"
#include "DefaultViewcontextManager.h"
#include "SuriObjectGroupManager.h"
#include "suri/ViewcontextTreeSelectionManager.h"
#include "ApplicationViewportManager.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/filename.h"
#include "wx/log.h"
#include "wx/listbook.h"
#include "wx/imaglist.h"
#include "wx/xrc/xmlres.h"
#include "wx/dir.h"
#include "PatFile.h"
#ifdef __WXMSW__
#include "wx/listctrl.h"
#endif

//Includes App
#include "Context2D.h"
#include "Context3D.h"
#include "ContextHtml.h"
#include "messages.h"
#include "resources.h"
#include "PatManagerWidget.h"
#include "PatFile.h"
#include "suri.h"
#include "MainWindow.h"

//Defines

//Forward

START_IMPLEMENT_EVENT_OBJECT(MainWindowEvent, MainWindow)
   IMPLEMENT_EVENT_CALLBACK(OnListbookChanged, OnListbookChanged(event),
         wxListbookEvent)
   IMPLEMENT_EVENT_CALLBACK(OnClose, OnClose(event), wxCloseEvent)
   IMPLEMENT_EVENT_CALLBACK(OnContextMenu, OnContextMenu(event), wxMenuEvent)
END_IMPLEMENT_EVENT_OBJECT

/**
 * Con este metodo se emula un singleton, para mantener una sola
 * instancia de la clase.
 * @return referencia a la unica instancia de la clase.
 */
MainWindow& MainWindow::Instance() {
   static MainWindow* ptool = new MainWindow();
   return *ptool;
}

/**
 * Crea la ventana principal (un listbook).
 * Crea y agrega los contextos:
 *  - Imagen (Raster con herramientas)
 *  - Mapa (Raster con herramientas)
 *  - 3D (OpenGL)
 *  - Documentos (HTML)
 *  - Fotografias (imagenes en HTML)
 *  - Actividades (HTML)
 * @return true siempre.
 */
bool MainWindow::CreateToolWindow() {
   // Tamanio minimo deseado
   wantedWidth_ = 400;
   wantedHeight_ = 300;

   wxListbook *plistbook = new wxListbook(pParentWindow_, wxID_ANY, wxDefaultPosition,
                                          wxDefaultSize, wxTAB_TRAVERSAL | wxLB_TOP,
                                          wxT("ID_MAINBOOK"));
   if (plistbook == NULL) {
      REPORT_DEBUG("Error al crear el listbook que contendra los contextos");
      return false;
   }
   pListBook_ = plistbook;
   pToolWindow_ = plistbook;
   // Cambio el color de fondo de la lista
   plistbook->GetListView()->SetBackgroundColour(
         wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
#ifdef __WXMSW__
   // Cambio el borde de la lista
   plistbook->GetListView()->SetSingleStyle(wxBORDER_NONE);
   plistbook->GetListView()->Refresh();
#endif   // __WXMSW__
   plistbook->Connect(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED,
                      wxListbookEventHandler(MainWindowEvent::OnListbookChanged), NULL,
                      pEventHandler_);
   if (GetFrame())
      GetFrame()->Connect(wxEVT_CLOSE_WINDOW,
                          wxCloseEventHandler(MainWindowEvent::OnClose), NULL,
                          pEventHandler_);
   pDatasourceManager_ = new suri::DefaultDatasourceManager;
   pGroupManager_ = new suri::SuriObjectGroupManager(pDatasourceManager_);
   pViewcontextManager_ = new suri::DefaultViewcontextManager(pDatasourceManager_);
   pLibraryManager_ = suri::LibraryManagerFactory::CreateLibraryManager();
   pSelectionManager_ = new suri::ViewcontextTreeSelectionManager(pGroupManager_,
                                                                  pViewcontextManager_);
   pFeatureSelection_ = new suri::FeatureSelection();
   pViewportManager_ = new ApplicationViewportManager(this);
   pTaskManager_ = new suri::TaskManager();
   pDataViewManager_ = new suri::DataViewManager(pDatasourceManager_,
                                                 pViewcontextManager_,
                                                 pViewportManager_, pGroupManager_,
                                                 pSelectionManager_, pLibraryManager_,
                                                 pTaskManager_, pFeatureSelection_);
   AddContext(new Context2D(pToolWindow_, pDataViewManager_), NAME_PROPERTY_VALUE_RASTER,
              caption_RASTER_CONTEXT, wxT(icon_RASTER_CONTEXT));

   AddContext(new Context2D(pToolWindow_, pDataViewManager_, Context::ContextMapType),
              NAME_PROPERTY_VALUE_MAP, caption_MAP_CONTEXT, wxT(icon_MAP_CONTEXT));

   AddContext(new Context3D(pToolWindow_, pDataViewManager_), NAME_PROPERTY_VALUE_TERRAIN,
              caption_TERRAIN_CONTEXT, wxT(icon_TERRAIN_CONTEXT));

   AddContext(new ContextHtml(pToolWindow_, pDataViewManager_, Context::ContextDocumentsType),
              NAME_PROPERTY_VALUE_DOCUMENTS, caption_DOCUMENTS_CONTEXT,
              wxT(icon_DOCUMENTS_CONTEXT));

   AddContext(new ContextHtml(pToolWindow_, pDataViewManager_, Context::ContextMultimediasType),
              NAME_PROPERTY_VALUE_MULTIMEDIA, caption_MULTIMEDIA_CONTEXT,
              wxT(icon_PHOTOGRAPHS_CONTEXT));

   AddContext(new ContextHtml(pToolWindow_, pDataViewManager_, Context::ContextActivitiesType),
              NAME_PROPERTY_VALUE_ACTIVITIES, caption_ACTIVITIES_CONTEXT,
              wxT(icon_ACTIVITIES_CONTEXT));

   suri::ProgressManager::SetCreateFunction(suri::WxProgressManager);
   return true;
}

/**
 *  Regresa el titulo de la ventana
 */
const wxString MainWindow::GetWindowTitle() const {
   return wxT("");
}

/**
 * Setea el titulo de la ventana principal de la app. Utilizado para agregar el
 * nombre del PAT con el que se esta trabajando.
 * @param[in]   Title: titulo de la ventana.
 */
void MainWindow::SetWindowTitle(const wxString &Title) {
   windowTitle_ = suri::Configuration::GetParameter("app_name", windowTitle_);
   windowTitle_ += wxT(" - ");
   if (!Title.IsEmpty())

      windowTitle_ += _(Title);
   else
      windowTitle_ += _(caption_BLANK_PAT);

   GetFrame()->SetTitle(windowTitle_);
   return;
}

/**
 * Retorna un bitmap con el icono de la ventana.
 * @param[in]   X Ancho deseado del bitmap
 * @param[in]   Y Alto deseado del bitmap
 * @param[out]   ToolBitmap: bitmap de la app.
 * \todo Implementar el uso de X e Y.
 */
void MainWindow::GetWindowIcon(wxBitmap& ToolBitmap, int X, int Y) const {
   GET_BITMAP_RESOURCE(wxT(icon_APPLICATION), ToolBitmap);
}

/**
 * Agrega la ventana asociada del contexto al listbook.
 * @param[in] pContext contexto a agregar
 * @param[in] ContextName nombre del contexto para el xml
 * @param[in] ContextLabel nombre del contexto para el usuario.
 * @param[in] Bitmap Icono a utilizar en el control junto con ContextLabel
 * @return True en caso de poder agregar el contexto.
 */
bool MainWindow::AddContext(Context *pContext, const wxString &ContextName,
                            const wxString &ContextLabel, const wxBitmap &Bitmap) {
   if (ContextName.IsEmpty())
   REPORT_AND_FAIL_VALUE("D: No se puede agregar contexto sin nombre", false);

   REPORT_DEBUG("D:Agregando contexto %s", ContextLabel.c_str());
   wxImageList *pimglist = pListBook_->GetImageList();
   if (!pimglist) {
      pimglist = new wxImageList(Bitmap.GetWidth(), Bitmap.GetHeight(), 1);
      pListBook_->AssignImageList(pimglist);
   }
   int imgidx = pimglist->Add(Bitmap);
   wxPanel *ppanel = new wxPanel(pListBook_, wxID_ANY);
   bool result = pListBook_->AddPage(ppanel, _(ContextLabel), false, imgidx);
   AddControl(pContext, ppanel);
   // Lo agrega a la lista
   if (result) {
      contextList_.push_back(pContext);
      pContext->GetNode()->AddProperty(wxT(NAME_NODE), ContextName);
      REPORT_DEBUG("D:Contexto %s agregado con exito", ContextLabel.c_str());
      pContext->GetWindow()->Connect(wxEVT_COMMAND_MENU_SELECTED,
                                     wxMenuEventHandler(MainWindowEvent::OnContextMenu),
                                     NULL, pEventHandler_);
   } else
      REPORT_ERROR("D:Contexto %s no fue agregado", ContextLabel.c_str());
   return result;
}

/**
 * \overload
 * Funcion de conveniencia para agregar un Contexto obteniendo valores para
 *  el nombre e icono de Widget::GetWindowTitle() y Widget::GetWindowIcon()
 * Prepara los datos para llamar al metodo overload que agrega el contexto.
 * @param[in] pContext el contexto
 * @param[in] ContextName el nombre del contexto para el xml
 * @param[in] ContextLabel el nombre del contexto para el usuario.
 * @param[in] Bitmap: un string con el recurso de donde se obtiene el icono
 * @return True en caso de poder agregar el contexto.
 */
bool MainWindow::AddContext(Context *pContext, const wxString &ContextName,
                            const wxString& Label, const wxString& Bitmap) {
   if (ContextName.IsEmpty())
   REPORT_AND_FAIL_VALUE("D: No se puede agregar contexto sin nombre", false);

   wxString label = Label;
   if (Label.empty()) label = pContext->GetWindowTitle();
   wxBitmap bmp;
   if (Bitmap.empty())
      pContext->GetWindowIcon(bmp);
   else
      GET_BITMAP_RESOURCE(Bitmap.c_str(), bmp);

   return AddContext(pContext, ContextName, label, bmp);
}

/**
 * Abre la ventana del Administrador de PATs.
 * @return true.
 */
bool MainWindow::OpenPat() {
   if (!PatCanBeSaved()) return false;
   (new PatManagerWidget(GetFrame(), &contextList_, NULL, pDataViewManager_))->ShowModal();
   return true;
}

/**
 * Verifica si el PAT cargado sufrio cambios comparando los strings que representan
 * al PAT en archivo con el PAT en memoria.
 * Si el PAT fue modificado carga la ventana para salvar el PAT.
 * @return true si el usuario presiona OK en el dialogo de salvar PAT. false C.O.C.
 * \todo No deberia usar IsPatModified?
 */
bool MainWindow::SavePat() {
   if (!PatCanBeSaved()) return false;

   bool userpat = false;
   userpat = pathPat_.IsEmpty() || pathPat_.StartsWith(GetPathUserPat().c_str());

   wxString oldname = "";
   if (!pathPat_.IsEmpty()) {
         wxFileName fn(pathPat_);
         wxString pathaux = fn.GetVolume().c_str()
               + (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
         pathaux += fn.GetFullPath(wxPATH_UNIX);
         PatFile pat(pathaux.c_str());
         if (pat.GetPatType() == PatFile::PatInvalid)
         REPORT_AND_FAIL_VALUE("D:Error al crear PatFile", false);
         oldname = pat.GetPatName();
   }
   wxXmlNode *pinfonode = GetInfoNode();
   PatInfoPart *ppatinfo = new PatInfoPart(pinfonode, &contextList_, userpat, pDataViewManager_,
                                           oldname,
                                           false, true);
   wxString xmldesc;
   if (!ppatinfo->CreatePat(xmldesc, pinfonode))
   REPORT_DEBUG("D:ERROR al obtener xml");
   delete pinfonode;

   suri::PartContainerWidget *ppart = new suri::PartContainerWidget(
         ppatinfo, _(caption_SAVE_PAT));

   if (!initPatXml_.IsEmpty() && xmldesc.Cmp(initPatXml_) != 0)
      ppatinfo->SetModified(true);
   return ppart->ShowModal() == wxID_OK;
}

/**
 * Este metodo cierra el PAT cargado, preguntando si se desean salvar los
 * cambios en caso de que los hubiera.
 * @return true.
 */
bool MainWindow::ClosePat() {
   if (!PatCanBeSaved()) return false;

   if (initPatXml_.IsEmpty() || IsPatModified()) {
      if (SHOWQUERY(pToolWindow_, _(question_SAVE_CHANGES),
            wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION) == wxID_YES) SavePat();
   }

   if (!CloseElements())
   SHOW_ERROR(message_CLOSE_ELEMENTS_ERROR);

   return true;
}

/**
 * Al tener una lista de contextos (tipo XmlElement) para generar las
 * propiedades necesita recorrer esta lista y obtener los nodos de cada uno.
 * @return puntero a un nodo con el xml de todos los contextos.
 * \attention quien invoque a este metodo es responsable del puntero devuelto,
 * por lo tanto debe hacerse cargo de su eliminacion.
 */
wxXmlNode *MainWindow::GetProperties() {
   if (!pXmlProperties_)
   REPORT_AND_RETURN_VALUE("D:Salida por pXmlProperties_ == NULL", NULL);

   std::list<Context*>::const_iterator it = contextList_.begin();
   while (it != contextList_.end()) {
      AddNode(pXmlProperties_, (*it)->GetProperties());
      it++;
   }

   wxXmlNode *pnodeprop = new wxXmlNode(*pXmlProperties_);

   while (wxXmlNode *pnode = pXmlProperties_->GetChildren()) {
      if (!pXmlProperties_->RemoveChild(pnode))
      REPORT_DEBUG("D:No se pudo eliminar el nodo");
      delete pnode;
   }

   return pnodeprop;
}

/**
 * Guarda una copia del nodo Informacion del PAT activo.
 * @param[in] pInfoNode: puntero al nodo informacion.
 */
void MainWindow::SetInfoNode(const wxXmlNode *pInfoNode) {
   if (pInfoNode_) {
      delete pInfoNode_;
      pInfoNode_ = NULL;
   }

   if (pInfoNode)
      pInfoNode_ = new wxXmlNode(*pInfoNode);
   else
      pInfoNode_ = NULL;

}

/**
 * Guarda el path del PAT activo(cargado).
 * @param[in]   Path: path del PAT activo.
 */
void MainWindow::SetPathPat(const std::string &Path) {
   if (!pathPat_.empty()) pathPat_.clear();
   pathPat_ = Path;
}

/**
 * Devuelve una copia del nodo informacion del PAT activo.
 * @return   NULL si no hay un PAT activo o una copia del Nodo Informacion del
 * PAT activo.
 * \attention quien invoque a este metodo es responsable del puntero devuelto,
 * por lo tanto debe hacerse cargo de su eliminacion.
 */
wxXmlNode * MainWindow::GetInfoNode() {
   if (!pInfoNode_) return NULL;
   return new wxXmlNode(*pInfoNode_);
}

/**
 * Guarda un xml con el estado inicial del PAT cargado.
 * @param[in]   DescXml:  xml del PAT cargado.
 */
void MainWindow::SetInitPatXml(const wxString &DescXml) {
   initPatXml_.Clear();
   initPatXml_ = DescXml;
}

/**
 * Devuelve el path de directorio donde se guardan los PATs de usuario.
 * @return string con el path del directorio de PATs de usuario.
 */
std::string MainWindow::GetPathUserPat() {
   pathUserPat_ = suri::Configuration::GetParameter("pat_save_dir", wxT("./"));
   return pathUserPat_;
}
/**
 * Devuelve el path de directorio donde se guardan los PATs instalados(propietarios).
 * @return string con el path del directorio de PATs propietarios.
 */
std::string MainWindow::GetPathRootPat() {
   pathRootPat_ = suri::Configuration::GetParameter("pat_install_dir", wxT("./"));
   return pathRootPat_;
}

/**
 * Objetivo: Cierra los elementos de todos los contextos.
 * @return todos cerrados? true:false;
 */
bool MainWindow::CloseElements() {
   if (!PatCanBeSaved()) return false;

   if (contextList_.empty()) return false;
   std::list<Context*>::const_iterator it;
   for (it = contextList_.begin(); it != contextList_.end(); it++)
      (*it)->CloseElements();
   SetInfoNode(NULL);
   SetPathPat("");
   SetInitPatXml("");
   SetPathRootPat("");
   SetPathUserPat("");
   SetWindowTitle("");
   pDataViewManager_->GetDatasourceManager()->Clear();
   pDataViewManager_->GetGroupManager()->Clear();
   pDataViewManager_->GetSelectionManager()->ResetSelection();
   // Actualiza la lista de cada contexto
   for (it = contextList_.begin(); it != contextList_.end(); it++) {
      (*it)->ForceListUpdate();
      (*it)->ForceViewerUpdate();
   }

   if (!ResetTempDir())
   REPORT_AND_FAIL_VALUE("D:No se pudo resetear el directorio temporal.", false);

   return true;
}

/**
 * Objetivo: Determina si hubo cambios o no en el PAT activo comparando
 * el string con el estado inicial del PAT cargado y un string con el estado
 * actual del PAT.
 * @return true PAT modificado.
 * @return false PAT sin modificar.
 */
bool MainWindow::IsPatModified() {
   bool userpat = false;
   if (pathPat_.IsEmpty() || pathPat_.StartsWith(GetPathUserPat().c_str())) userpat =
         true;
   PatInfoPart *ppatinfo = new PatInfoPart(GetInfoNode(), &contextList_, userpat, pDataViewManager_);
   wxString xmldesc;
   if (!ppatinfo->CreatePat(xmldesc, GetInfoNode()))
   REPORT_DEBUG("D:ERROR al obtener xml");

   delete ppatinfo;
   return (xmldesc.Cmp(initPatXml_) != 0);
}

/**
 * Este metodo se ejecuta cuando la ventana recibe un evento de close.
 * Da la opcion de guardar el PAT en caso de que los contextos esten modificados.
 * El usuario puede cancelar la accion.
 * \post En caso de salida blanquea las variables(volatile) del archivo de configuracion.
 * \post En caso de salida, elimina el directorio temporal.
 * @param[in]   Event: evento.
 */
void MainWindow::OnClose(wxCloseEvent &Event) {
   if (!PatCanBeSaved()) {
      Event.Skip(false);
      return;
   }

   bool shouldsave = false;
   if (GetPathPat().IsEmpty()) {
      /*esto es en el caso de que no haya 1 PAT cargado y se cargue 1 elemento
       * manualmente en algun contexto.*/
      std::list<Context*>::const_iterator it;
      for (it = contextList_.begin(); it != contextList_.end() && !shouldsave; it++)
         if ((*it)->GetElementList()->GetElementCount() > 0) shouldsave = true;
   } else
      shouldsave = IsPatModified();

   if (shouldsave) {
      switch(SHOWQUERY(GetWindow(), _(question_SAVE_CHANGES),
                                       wxYES_NO|wxCANCEL|wxICON_QUESTION))
            {
               case wxID_YES:
               SavePat();
               break;
               case wxID_CANCEL:
               Event.Skip(false);
               break;
               default:
               break;
            }
         }
         // Si me voy, vuelo el volatile
   if (Event.GetSkipped()) {
      if (!CloseElements())
      REPORT_DEBUG("D:Error al intentar cerrar elementos.");

      //elimino el directorio temporal de la aplicacion.
      std::string pathtemp = suri::Configuration::GetParameter("app_temp_dir",
                                                               wxT("./"));
      wxFileName fn(pathtemp);
      if (!suri::RemoveDir(fn))
      REPORT_DEBUG("D:Error al intentar eliminar directorio temporal.");

      suri::Configuration::SetParameter("app_base_dir_volatile", "");
      suri::Configuration::SetParameter("app_hotlink_base_dir_volatile", "");
      suri::Configuration::SetParameter("app_vector_base_dir_volatile", "");
   }
}

/*!
 *  Evento de menu contextual sobre la lista. Provee la funcionalidad de copia
 * de elementos a otros contextos.
 * @param Event Evento de menu
 */
void MainWindow::OnContextMenu(wxMenuEvent &Event) {
   suri::LayerList *plist = GetActiveContext()->GetElementList();
   suri::Element *pselectedelement =
         plist ? plist->GetElement(
                       plist->GetSelectedElementUid(suri::LayerList::AllGroups)) :
                 NULL;
   if (pselectedelement) {
      wxXmlNode *pelementnode = pselectedelement->GetProperties();
      Context::ContextType type = Context::ContextErrorType;
      if (Event.GetId() == XRCID("ID_SENDTO_IMAGE")) {
         type = Context::Context2DType;
      } else if (Event.GetId() == XRCID("ID_SENDTO_MAP")) {
         type = Context::ContextMapType;
      } else if (Event.GetId() == XRCID("ID_SENDTO_3D")) {
         type = Context::Context3DType;
      }
      if (!GetContext(type)->AddElement(pelementnode)) {
         SHOW_ERROR(message_COPY_ELEMENT_ERROR);
         REPORT_AND_FAIL(message_COPY_ELEMENT_ERROR);
      }
   }
}

/**
 * Limpia el directorio temporal de la aplicacion.
 * Recrea la estructura de directorios dentro del mismo.
 * @return true en caso de exito.
 * @return false si no pudo eliminar algun directorio.
 * @return false si no pudo recrear algun directorio.
 */
bool MainWindow::ResetTempDir() {
   //Borro el dir. temporal
   std::string tempdir = suri::Configuration::GetParameter("app_temp_dir", wxT(""));
   wxFileName fn(tempdir);
   // si el directorio temporal esta vacio (condicion default del GetParameter)
   // o hubo un error al eliminar el directorio, reporta el error.
   if (tempdir.empty() || !suri::RemoveDir(fn))
   REPORT_DEBUG("D:Error al intentar eliminar directorio temporal.");
   //seteo la variable que contiene el dir. de los hotlinks
   std::string common = suri::Configuration::GetParameter("pat_common_dir", wxT(""));
   std::string temphotlinksdir = tempdir + common
         + suri::Configuration::GetParameter("app_hotlinks_dir", wxT(""));
   suri::Configuration::SetParameter("app_hotlink_base_dir_volatile", temphotlinksdir);

   std::string tempvectordir = tempdir + common
         + suri::Configuration::GetParameter("app_vector_dir", wxT(""));
   suri::Configuration::SetParameter("app_vector_base_dir_volatile", tempvectordir);

   wxFileName fnhl(temphotlinksdir);
   if (!fnhl.Mkdir(0755, wxPATH_MKDIR_FULL)) {
      SHOW_ERROR(message_HOTLINK_DIR_CREATION_ERROR);
      //no se traduce, lo hace SHOW_
      return false;
   }

   wxFileName fnvec(tempvectordir);
   if (!fnvec.Mkdir(0755, wxPATH_MKDIR_FULL)) {
      SHOW_ERROR(message_VECTOR_DIR_CREATION_ERROR);
      //no se traduce, lo hace SHOW_
      return false;
   }

   return true;
}

/**
 * Determina si un contexto fue modificado.
 * @return true No hay PAT cargado, pero tengo elementos en los contextos
 * @return true Tengo un PAT cargado que fue modificado
 */
bool MainWindow::IsContextModified() {
   // Si no tengo un PAT cargado y tengo en algun contexto algun elemento
   if (GetPathPat().IsEmpty()) {
      std::list<Context*>::iterator it = contextList_.begin();
      for (; it != contextList_.end(); it++)
         if ((*it)->GetElementList()->GetElementCount()) return true;
   }
    else
       return IsPatModified();

   return false;
}

/**
 * Invoca al metodo InstallCompressedPat para instalar el PAT.
 * @param[in]   CommpressedPatUrl: url del PAT comprimido a instalar.
 * \note este metodo es invocado desde la clase Application
 */
void MainWindow::SetParameter(const wxString &CommpressedPatUrl) {
   if (!CommpressedPatUrl.IsEmpty()) InstallCommpressedPat(CommpressedPatUrl);
}

/**
 * Crea un PatManagerWidget e instala el PAT cuyo url recibe por parametro.
 * @param[in]   CommpressedPatUrl: url del PAT comprimido a instalar.
 */
void MainWindow::InstallCommpressedPat(const wxString &CommpressedPatUrl) {
   PatManagerWidget patmanager(GetFrame(), &contextList_, NULL, pDataViewManager_);
   patmanager.Install(CommpressedPatUrl);
   patmanager.ShowModal();
}

/**
 * Informa si alguno de los contextos tiene un elemento en edicion
 */
bool MainWindow::GetEdited() {
   std::list<Context*>::iterator it = contextList_.begin();
   for (; it != contextList_.end(); it++) {
      if ((*it)->GetElementList()->GetEditedCount() > 0) return true;
   }
   return false;
}

/**
 * Informa si alguno de los contextos tiene un elemento en edicion
 */
bool MainWindow::PatCanBeSaved() {
   if (GetEdited()) {
      if (SHOWQUERY(pToolWindow_, _(question_EDITED_ELEMENTS),
            wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION) == wxID_YES) {
         std::list<Context*>::iterator it = contextList_.begin();
         for (; it != contextList_.end(); it++) {
            if ((*it)->GetElementList()->GetEditedCount() > 0) {
               suri::LayerList::ElementListType::iterator it2 =
                     (*it)->GetElementList()->GetElementIterator();
               while (it2 != (*it)->GetElementList()->GetElementIterator(false))
                  if ((*it2)->IsEdited())
                     it2 = (*it)->GetElementList()->DelElement(it2);
                  else
                     it2++;
            }
         }
      } else
         return false;
   }
   return true;
}

/**
 * Retorna el contexto activo. Si no encuentra uno retorna NULL
 * @return Contexto activo. NULL si no encuentra uno.
 */
Context* MainWindow::GetActiveContext() {
   //Busca la posicion del contexto activo
   if (pListBook_ == NULL) {
      REPORT_DEBUG("No se creo la herramienta que contiene contextos");
      return NULL;
   }
   int activecontextposition = pListBook_->GetSelection();

   //Recorre la lista de contextos y retorna el activo
   std::list<Context*>::const_iterator it = contextList_.begin();
   for (int i = 0; i < activecontextposition && it != contextList_.end(); i++, it++)
      ;
   if (it == contextList_.end()) {
      REPORT_DEBUG("Error al buscar el contexto activo");
      return NULL;
   }
   return *it;
}

/** Retorna el contexto por tipo */
Context *MainWindow::GetContext(const Context::ContextType &Type) const {
   //Busca la posicion del contexto activo
   if (pListBook_ == NULL) {
      REPORT_DEBUG("No se creo la herramienta que contiene contextos");
      return NULL;
   }
   //Recorre la lista de contextos y retorna el activo
   std::list<Context*>::const_iterator it = contextList_.begin();
   for (; (*it)->GetContextType() != Type && it != contextList_.end(); ++it) {
   }
   if (it == contextList_.end()) {
      REPORT_DEBUG("Error al buscar el contexto");
      return NULL;
   }
   return *it;
}

/**
 * Este metodo se ejecuta cuando el listbook de la ventana recibe un evento de
 * cambio de pagina.
 * Antes de cambiar de contexto, desabilita dicha pestania y habilita la
 * pestania a la que se esta moviendo.
 * \todo Se busca que los contextos no visibles dejen de procesar eventos, y por
 *   consiguiente dejar de renderizar, etc. Este acercamiento no funciona, evaluar
 *   otras posibilidades.
 * @param[in]   Event: evento
 */
void MainWindow::OnListbookChanged(wxListbookEvent &Event) {
   int previouscontext = Event.GetOldSelection();
   int nextcontext = Event.GetSelection();
   REPORT_DEBUG("D:Paso de contexto %d -> %d", previouscontext, nextcontext);
   std::list<Context*>::iterator it = contextList_.begin();
   int count = 0;
   while (count < previouscontext && it != contextList_.end()) {
      count++;
      it++;
   }
   if (it == contextList_.end())
      return;

   REPORT_DEBUG("D:Frizando contexto %s", (*it)->GetWindowTitle().c_str());
   (*it)->GetWindow()->Freeze();
   (*it)->GetWindow()->Enable(false);
   it = contextList_.begin();
   count = 0;
   while (count < nextcontext && it != contextList_.end()) {
      count++;
      it++;
   }
   if (it == contextList_.end())
      return;
   pViewcontextManager_->SelectViewcontext(
         (*it)->GetSelectedViewcontext()->GetViewcontextId());
   REPORT_DEBUG("D:Derritiendo contexto %s", (*it)->GetWindowTitle().c_str());
   (*it)->GetWindow()->Enable(true);
   if ((*it)->GetWindow()->IsFrozen()) (*it)->GetWindow()->Thaw();
}

/**
 * Constructor.
 */
MainWindow::MainWindow() :
      Widget(NULL), XmlElement(wxT(PAT_NODE), wxT(TYPE_PROPERTY_VALUE_PAT_ABSOLUTE)),  //No se traduce, elemento de XML
      NEW_EVENT_OBJECT(MainWindowEvent)
{
   initPatXml_ = wxT("");
   pListBook_ = NULL;
   pInfoNode_ = NULL;
   pathPat_ = wxT("");
   pathRootPat_ = wxT("");
   pathUserPat_ = wxT("");
   CreateTool();
   CloseElements();
}

/**
 * Destructor.
 */
MainWindow::~MainWindow() {
   wxLog::DontCreateOnDemand();
   delete wxLog::SetActiveTarget(NULL);
   delete pInfoNode_;
   // elimino el helper de eventos
   DELETE_EVENT_OBJECT
   ;
}

//-------------------------------------------------------------------------
/**
 * Suri::VectorElementEditor requiere que se definan las funciones
 * globales DeleteElement y AddElement para que la funcionalidad de
 * edicion de vectores pueda estar en la biblioteca.
 * En SOPI estas funciones estan definidas en ContextManager y utilizan la
 * instancia del mismo para agregar y eliminar elementos de todos los contextos.
 * En Seymour esta clase no existe, por esta razon las funciones estaran en
 * MainWindow. Se agrega MainWindow::GetActiveContext que retorna el contexto
 * activo en ese momento. El objetivo es que cada vez que se llama a las
 * funciones se agregue/elimine el elemento del contexto activo.
 */
/**
 * Agrega un elemento a partir de su nodo xml.
 * @param[in] pElementNode nodo del elemento a agregar.
 * @return el resultado de invocar a ::AddElement.
 */
extern "C" bool AddElement(wxXmlNode *pElementNode) {
   Context* pactivecontext = MainWindow::Instance().GetActiveContext();
   return (pactivecontext != NULL && pactivecontext->AddElement(pElementNode) != NULL);
}

/**
 * Elimina un elemento a partir de su identificador unico.
 * @param[in] ElementUid uid del elemento a agregar
 * @return el resultado de invocar a ::DeleteElement.
 */
extern "C" bool DeleteElement(const suri::Element::UidType& ElementUid) {
   Context* pactivecontext = MainWindow::Instance().GetActiveContext();
   return (pactivecontext != NULL && pactivecontext->GetElementList()
         && pactivecontext->GetElementList()->DelElement(ElementUid));
}

/**
 * Elimina una lista de elementos a partir del identificador unico de cada uno.
 * @param[in] ElementUid lista de uid de los elementos a agregar
 * @return el resultado de invocar a ::DeleteElements.
 */
extern "C" bool DeleteElements(const std::list<suri::Element::UidType>& ElementUid) {
   Context* pactivecontext = MainWindow::Instance().GetActiveContext();
   return (pactivecontext != NULL && pactivecontext->GetElementList()
         && pactivecontext->GetElementList()->DelElement(ElementUid.begin(),
                                                         ElementUid.end()) > 0);
}

