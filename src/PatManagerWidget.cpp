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

// <COPYRIGHT> a reemplazar a futuro

// Includes standard

// Includes Suri
#include "logmacros.h"
#include "PatFile.h"
#include "suri/AuxiliaryFunctions.h"
#include "suri/Configuration.h"
#include "suri/xmlnames.h"
#include "suri/FileManagementFunctions.h"
#include "suri/DataViewManager.h"
#include "ContextHandlerInterface.h"
#include "ContextHandlerFactory.h"
#include "PatManagerWidget.h"

//Includes Wx
#include "wx/xrc/xmlres.h"
#include "wx/splitter.h"
#include "wx/dir.h"
#include "wx/sstream.h"
#include "wx/textfile.h"
#include "wx/imaglist.h"
#include <wx/progdlg.h>

//Includes App
#include "MainWindow.h"
#include "messages.h"
#include "resources.h"
#include "ApplicationWindowManager.h"

//Defines
//! muestra mensaje con los elementos no cargados del PAT.
#define __SHOW_NOT_LOADED_FILES_TO_USER__
//! defino que no permita PATs creados con una version de aplicacion menor o igual a __PAT_INCOMPATIBLE_VERSION_STRING__
#undef CHECK_FIXED_PAT_VERSION
//! defino que no permita PATs creados con aplicaciones posteriores a la actual
#define CHECK_APPLICATION_VS_PAT_VERSION
//! string contra el que compara (ordenamiento de strings) para verificar la compatibilidad del PAT con la aplicacion
#define __PAT_INCOMPATIBLE_VERSION_STRING__ "1.0 build 3803"

/* Macro para eliminar el archivo temporal generado al momento de generar un
 * PAT comprimido.
 */
#define REMOVE_PAT_TEMP_DIR(UrlTempFile, Pat, ShowMsgError)do{                \
   if(!wxRemoveFile(UrlTempFile))                                             \
      REPORT_DEBUG(message_DELETE_FILE_ERROR_s, UrlTempFile.c_str());         \
   if(ShowMsgError)                                                           \
      SHOW_ERROR(message_COMPRESSED_PAT_GENERATION_ERROR,                     \
                                                   Pat.GetErrorMsg().c_str());\
   return;                                                                    \
}while(0)

/* Macro para obtener nodo cuyo nombre es NameParentNode y devolver su primer
 * nodo hijo.
 * \note el nodo hijo lo devuelve sobre el mismo puntero que se pasa con el nodo
 * padre. */
#define GET_FIRST_CHILD(pParentNode, NameParentNode)do{                       \
   while(pParentNode && pParentNode->GetName().Cmp(wxT(NameParentNode))!=0)   \
      pParentNode = pParentNode->GetNext();                                   \
   if(pParentNode)                                                            \
      pParentNode = pParentNode->GetChildren();                               \
   else                                                                       \
      pParentNode=NULL;                                                       \
}while(0)                                                                     \

//Forwards
void InitPatManagerXmlResource();

START_IMPLEMENT_EVENT_OBJECT(PatManagerWidgetEvent, PatManagerWidget)
   IMPLEMENT_EVENT_CALLBACK(OnButtonOk,OnButtonOk(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonCancel,OnButtonCancel(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonNew,OnButtonNew(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonEdit,OnButtonEdit(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonLoad,OnButtonLoad(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonInstall,OnButtonInstall(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonDelete,OnButtonDelete(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonCompress,OnButtonCompress(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnButtonHelp,OnButtonHelp(event),wxCommandEvent)
   IMPLEMENT_EVENT_CALLBACK(OnItemSelected, OnItemSelected(event), wxListEvent)
   IMPLEMENT_EVENT_CALLBACK(OnItemDeselected, OnItemDeselected(event), wxListEvent)
   IMPLEMENT_EVENT_CALLBACK(OnItemActivated, OnItemActivated(event), wxListEvent)
   IMPLEMENT_EVENT_CALLBACK(OnKeyPress, OnKeyPress(event), wxKeyEvent)
   IMPLEMENT_EVENT_CALLBACK(OnUIUpdate, OnUIUpdate(event), wxUpdateUIEvent)
   IMPLEMENT_EVENT_CALLBACK(OnClose, OnClose(event), wxCloseEvent)
END_IMPLEMENT_EVENT_OBJECT

/**
 * Ctor.
 * \post Se saca una copia de pNodeInfo
 * @param[in]  pParentFrame puntero al Frame padre.
 * @param[in]  pContextList puntero a la lista de contextos.
 * @param[in]  pNodeInfo puntero al nodo informacion.
 * \todo Ver por que cambian las dimensiones finales entre linux/windows.
 *       El define sirve para que se vea bien, pero no deberia haber diferencia.
 */
PatManagerWidget::PatManagerWidget(wxTopLevelWindow *pParentFrame,
                                   std::list<Context*> *pContextList,
                                   wxXmlNode *pNodeInfo,
                                   suri::DataViewManager* pDataViewManager) :
      PartContainerWidget(_(caption_PAT_MANAGER), SUR_BTN_NONE, SUR_BTN_NONE),
      pPatProp_(NULL), pPatInfo_(NULL), pListContext_(pContextList), pImageList_(NULL),
      selectedPat_(wxT("")), pNodeInfo_(NULL), NEW_EVENT_OBJECT(PatManagerWidgetEvent),
      pDataViewManager_(pDataViewManager) {
   if (pNodeInfo)
      pNodeInfo_ = new wxXmlNode(*pNodeInfo);
   else
      pNodeInfo_ = NULL;
   wantedWidth_ = 600; ///< Ancho prefereido de ventana
   wantedHeight_ = 300; ///< Alto preferido de ventana
   // En GTK queda mejor con esta dimension
#ifdef __WXGTK__
   wantedWidth_ = 660; ///< Ancho prefereido de ventana
   wantedHeight_ = 450; ///< Alto preferido de ventana
#endif
   CreatePatDirs();
   CreateTool();
   RefreshPatList();
   if (GetFrame())
      GetFrame()->Center();
}

/**
 * Destructor.
 * Elimina el objeto de eventos.
 * Elimina el puntero:
 *  * al Nodo informacion.
 *  * a Lista de imagenes de la lista de PATs
 *  * a la ventana donde se cargan los datos del PAT.
 *  * a la ventana que muestra los datos del PAT.
 */
PatManagerWidget::~PatManagerWidget() {
   DELETE_EVENT_OBJECT;
   delete pNodeInfo_;
   // delete pImageList_;
   // delete pPatInfo_;
}

/**
 * Crea(carga) la ventana diseniada. Asocia eventos a los botones de la toolbar
 * que posee la ventana y a la lista donde se muestran los PATs. Tambien
 * inicializa la lista de imagenes que maneja la lista donde se muestran los PATs.
 * @return true.
 */
bool PatManagerWidget::CreateToolWindow() {
   pToolWindow_ = wxXmlResource::Get()->LoadPanel(pParentWindow_, wxT("ID_DLGOPENPAT"));
   // Conecto los controles con sus eventos.
   GET_CONTROL(*pToolWindow_, "wxID_OK", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonOk),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "wxID_OK", wxButton)->Connect(wxEVT_UPDATE_UI,
   wxUpdateUIEventHandler(PatManagerWidgetEvent::OnUIUpdate),
                                                            NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "wxID_CANCEL", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonCancel),
         NULL, pEventHandler_);
#ifdef __UNUSED_CODE__
   /* GET_CONTROL(*pToolWindow_, "wxID_CANCEL1", wxButton)->
    Connect(wxEVT_COMMAND_BUTTON_CLICKED,
    wxCommandEventHandler(PatManagerWidgetEvent::OnButtonCancel),
    NULL,   pEventHandler_);*/
#endif

   GET_CONTROL(*pToolWindow_, "ID_NEW_PAT", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonNew),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_EDIT_PAT", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonEdit),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_EDIT_PAT", wxButton)->Connect(wxEVT_UPDATE_UI,
   wxUpdateUIEventHandler(PatManagerWidgetEvent::OnUIUpdate),
                                                                NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_LOAD_PAT", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonLoad),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_INSTALL_PAT", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonInstall),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_DELETE_PAT", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonDelete),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_DELETE_PAT", wxButton)->Connect(wxEVT_UPDATE_UI,
   wxUpdateUIEventHandler(PatManagerWidgetEvent::OnUIUpdate),
                                                                  NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_COMPRESS_PAT", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonCompress),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_COMPRESS_PAT", wxButton)->Connect(wxEVT_UPDATE_UI,
   wxUpdateUIEventHandler(PatManagerWidgetEvent::OnUIUpdate),
                                                                    NULL,
                                                                    pEventHandler_);
   GET_CONTROL(*pToolWindow_, "wxID_HELP", wxButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(PatManagerWidgetEvent::OnButtonHelp),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->Connect(
         wxEVT_COMMAND_LIST_ITEM_SELECTED,
         wxListEventHandler(PatManagerWidgetEvent::OnItemSelected),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->Connect(
         wxEVT_COMMAND_LIST_ITEM_DESELECTED,
         wxListEventHandler(PatManagerWidgetEvent::OnItemDeselected),
         NULL, pEventHandler_);
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->Connect(
         wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
         wxListEventHandler(PatManagerWidgetEvent::OnItemActivated),
         NULL, pEventHandler_);

   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->Connect(
         wxEVT_KEY_DOWN, wxKeyEventHandler(PatManagerWidgetEvent::OnKeyPress),
         NULL,
         pEventHandler_);

   // Para poder conectar el evento de cierre del dialogo.
   // pparent deberia ser un wxDialog.
   wxWindow* pparent = pToolWindow_->GetParent();
   wxDialog* pdialog = dynamic_cast<wxDialog*>(pparent);
   if (pdialog != NULL) {
      pdialog->Connect(wxEVT_CLOSE_WINDOW,
                       wxCloseEventHandler(PatManagerWidgetEvent::OnClose),
                       NULL, pEventHandler_);
   }

   //Agrego bitmaps a la lista de imagenes de la lista de PAT's.
   wxBitmap userbmp, rootbmp;
   wxIcon userico, rootico;
   GET_BITMAP_RESOURCE(wxT(icon_INSTALLED_PAT), userbmp);
   GET_BITMAP_RESOURCE(wxT(icon_USER_PAT), rootbmp);
   userico.CopyFromBitmap(userbmp);
   rootico.CopyFromBitmap(rootbmp);
   pImageList_ = new wxImageList(16, 16, true);
   pImageList_->Add(wxIcon(userico));
   pImageList_->Add(wxIcon(rootico));
   return true;
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * OK de la ventana. Dependiendo de la Part activa al momento de clickear, se
 * llevan a cabo 2 acciones diferentes:
 *  - Si no se esta mostrando el Part para editar las propiedades del PAT,
 *  cierra la ventana (PatManagerWidget).
 *  - Si se esta mostrando el Part para editar las propiedades del PAT, se
 *  intenta aplicar los cambios realizados y en caso de exito se cierra la Part
 *  de edicion de propiedades para mostrar la Part de Visualizacion de Propiedades.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonOk(wxCommandEvent &ButtonEvent) {
   if (!pPatInfo_) {
      suri::PartContainerWidget::OnButtonCancel(ButtonEvent);
   } else {
      ButtonEvent.Skip(false);
      //Si puede aplicar los cambios
      if (ApplyChanges()) {
         //Refresca la lista de pats
         RefreshPatList();
         //Oculta el patinfo si existe y despliega el patproperties
         ShowPatProperties();
      }
      EndModal(wxID_OK);
   }
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * Cancel de la ventana.
 * Este metodo tiene la logica para comportarse de 2 formas diferentes segun se
 * este mostrando o no la ventana de edicion de propiedades del PAT:
 *  - Si se esta mostrando el Part para editar las propiedades del PAT, se
 *  invoca al metodo PartContainerWidget::OnButtonCancel, ya que tiene la logica
 *  para validar el cerrado de la ventana preguntando si se desean guardar o no
 *  los cambios realizados.
 *  - si no se esta mostrando el Part para editar las propiedades del PAT, se
 *  cierra la ventana (PatManagerWidget). *
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonCancel(wxCommandEvent &ButtonEvent) {
   /* si el PatInfoPart(Edicion de propiedades del PAT) se esta mostrando, ejecuto
    * el evento Cancel del PartContainerWidget, sino dejo el comportamiento estandar*/
   if (pPatInfo_) {
      suri::PartContainerWidget::OnButtonCancel(ButtonEvent);
      if (ButtonEvent.ShouldPropagate()) {
         RefreshPatList();
         ButtonEvent.StopPropagation();
      }
   }
   EndModal(wxID_CANCEL);
}

/**
 * Este metodo responde a los eventos de click que se souceden sobre el boton
 * "Nuevo Pat" de la ventana.
 * Prepara las variables para crear un PAT y llama al metodo que muestra la
 * ventana de edicion de propiedades del PAT.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonNew(wxCommandEvent &ButtonEvent) {
   REPORT_DEBUG("D:Nuevo PAT");
   if (pNodeInfo_) {
      delete pNodeInfo_;
      pNodeInfo_ = NULL;
   }

   PatFile pat(selectedPat_);
   if (pat.GetPatType() != PatFile::PatInvalid) {
      long itemidx = USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl,
            FindItem(-1, pat.GetPatName()), -1);
      GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->SetItemState(
            itemidx, 0, wxLIST_STATE_SELECTED);
   }
   selectedPat_.clear();
   ShowPatInfo();
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * "Editar PAT" de la ventana.
 * Verifica que sea un PAT de usuario el que se quiere editar y llama al metodo
 * que muestra la ventana de edicion de propiedades del PAT.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonEdit(wxCommandEvent &ButtonEvent) {
   bool error = false;
   if (selectedPat_.empty() || !USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT",
         wxListCtrl, GetSelectedItemCount(), 0)) {
      SHOW_ERROR(message_SELECT_PAT);
      //no se traduce, lo hace SHOW_
      error = true;
   }

   if (!IsUserPat(selectedPat_)) {
      SHOW_ERROR(message_CANNOT_EDIT_INSTALLED_PAT);
      //No se traduce, lo hace SHOW_
      error = true;
   }

   if (error) {
      ShowPatProperties();
      return;
   }

   ShowPatInfo(true);
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * "Cargar PAT" de la ventana.
 * Invoca a un metodo que carga los elementos del PAT en sus respectivos
 * contextos, modifica el titulo de la ventana principal de la app con el nombre
 * del PAT.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonLoad(wxCommandEvent &ButtonEvent) {
   DoLoad();
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * "Instalar PAT" de la ventana.
 * Crea un dialogo de apertura de archivos para poder seleccionar el PAT
 * comprimido, luego invoca al metodo Install para que instale el PAT seleccionado.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonInstall(wxCommandEvent &ButtonEvent) {
   //obtengo el path del .pat a instalar.
   wxFileDialog filedlg(pToolWindow_, _(caption_INSTALL_PAT), wxT(""), wxT(""),
   _(filter_PAT_FILES));
   if (filedlg.ShowModal() == wxID_CANCEL)
      return;

   Install(filedlg.GetPath());
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * "Eliminar PAT" de la ventana.
 * Verifica que haya un PAT seleccionado en la lista de PATs e invoca al metodo
 * que elimina dicho PAT.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonDelete(wxCommandEvent &ButtonEvent) {
   if (selectedPat_.empty() || !USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT",
         wxListCtrl, GetSelectedItemCount(), 0)) {
      SHOW_ERROR(message_SELECT_PAT);
      //no se traduce, lo hace SHOW_
      return;
   }

   DeletePat();
   return;
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * "Generar PAT" de la ventana.
 * Presenta un dialogo para seleccionar el directorio donde se quiere generar el
 * PAT comprimido. Verifica que en dicho directorio no exista otro PAT
 * comprimido con el mismo nombre. En caso de conflicto pregunta al usuario si
 * desea sobreescribir el archivo.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonCompress(wxCommandEvent &ButtonEvent) {
   if (selectedPat_.empty() || !USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT",
         wxListCtrl, GetSelectedItemCount(), 0)) {
      SHOW_ERROR(message_SELECT_PAT);
      //no se traduce, lo hace SHOW_
      return;
   }

   wxDirDialog dirdlg(pToolWindow_, _(caption_SELECT_PAT_DIR));
   if (dirdlg.ShowModal() != wxID_OK)
      return;

   wxFileName fnpathpat(dirdlg.GetPath().c_str());
   /* no hago la verificacion por wxFileName::IsDir() ya que si el path no
    * termina con un separador de directorio devuelve false. */
   if (!fnpathpat.DirExists()) {
      SHOW_ERROR(message_NOT_A_VALID_DIR, fnpathpat.GetFullPath().c_str());
      return;
   }

   std::string pathpat = CreateXmlTemp(selectedPat_.c_str());
   PatFile pat(pathpat);
   if (pat.GetPatType() == PatFile::PatInvalid)
      REMOVE_PAT_TEMP_DIR(pathpat, pat, true);

   wxString aux = fnpathpat.GetVolume().c_str()
         + (fnpathpat.HasVolume() ? fnpathpat.GetVolumeSeparator() : wxT(""));
   aux += fnpathpat.GetFullPath(wxPATH_UNIX);
   aux += wxFileName::GetPathSeparator(wxPATH_UNIX);
   aux += pat.GetPatName().c_str();
   aux += wxT(extension_DOT_PAT); //no se traduse por ser extension

   // verifico que haya espacio suficiente para poder crear el PAT.
   if (!pat.CheckPatSize(fnpathpat.GetPathWithSep().c_str()))
      REMOVE_PAT_TEMP_DIR(pathpat, pat, true);

   /* Verifico que no exista el .pat a generar. En caso de que exista pregunto
    * si quiere sobreescribir */
   if (wxFileName::FileExists(aux)) {
      if (SHOWQUERY(pToolWindow_, _(question_FILE_EXISTS), wxOK|wxCANCEL) != wxID_OK)
         REMOVE_PAT_TEMP_DIR(pathpat, pat, false);
   }

   REPORT_WARNING(
         "D:Se esta generando un PAT comprimido utilizando la version DEBUG del software. El PAT puede no ser compatible con los generados por otras versiones del software");
   FLUSH();

   if (!pat.CreatePatFile(aux.c_str()))
      REMOVE_PAT_TEMP_DIR(pathpat, pat, true);

   //si existe, elimino el archivo temporal. Ver que CreatePatFile elimina el archivo temporal.
   if (wxFileName(pathpat).FileExists())
      if (!wxRemoveFile(pathpat))
         REPORT_DEBUG(
               "D:Error al intentar eliminar archivo: %s. Verifique que exista y que no este siendo usado.",
               pathpat.c_str());

   // Verifico que el PAT se haya creado bien.
   if (!VerifyCompressedPat(aux.c_str()))
      SHOW_ERROR(message_PAT_GENERATION_ERROR_s, "");
   // No se traduce, lo hace SHOW_
   else
      SHOW_INFO(message_PAT_SUCCESFULL_GENERATION);
   // no se traduce. Lo hace SHOW_
   return;
}

/**
 * Este metodo responde a los eventos de click que se suceden sobre el boton
 * "Ayuda" de la ventana.
 * Abre la ventana de ayuda.
 * @param[in]  ButtonEvent evento.
 */
void PatManagerWidget::OnButtonHelp(wxCommandEvent &ButtonEvent) {
   ApplicationWindowManager::Instance().GetHelp(0);
}

//! Evento de seleccion de algun item de la lista.
/**
 * Este metodo responde a los eventos de seleccion de items sobre la lista de
 * PATs.
 * Cuando se selecciona un item (PAT):
 *  - se carga el Part PatPropertiesHtmlViewerPart en el espacio del control
 *  principal del PatManagerWidget, mostrando los datos contenidos en el nodo
 *  informacion del PAT.
 * @param[in]  ListEvent permite obtener el indice del elmento seleccionado
 */
void PatManagerWidget::OnItemSelected(wxListEvent &ListEvent) {
   if (ListEvent.GetIndex() < 0) {
      ShowPatProperties();
      return;
   }
   //armo el path del PAT seleccionado.
   std::string pathpat = MainWindow::Instance().GetPathUserPat();
   if (ListEvent.GetData() == 0)
      pathpat = MainWindow::Instance().GetPathRootPat();

   pathpat += ListEvent.GetText();
   pathpat += wxFileName::GetPathSeparator();
   pathpat += wxT("descripcion.xml");
   wxFileName fnpat(pathpat);
   selectedPat_ = fnpat.GetVolume().c_str()
         + (fnpat.HasVolume() ? fnpat.GetVolumeSeparator() : wxT(""));
   selectedPat_ += fnpat.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
   selectedPat_ += fnpat.GetFullName().c_str();

   PatFile pat(selectedPat_);
   if (pat.GetPatType() != PatFile::PatInvalid) {
      delete pNodeInfo_;
      pNodeInfo_ = pat.GetInfoNode();
      ShowPatProperties(selectedPat_);
   } else
      ShowPatProperties();

   ListEvent.Skip();
}

/**
 * Este metodo responde a los eventos de deseleccion de algun item de la lista.
 * Cuando se deselecciona un item (PAT):
 *  - se carga el Part PatPropertiesHtmlViewerPart en el espacio del control
 *  principal del PatManagerWidget, mostrando la leyenda
 *                                 "Seleccione un PAT para obtener informacion"
 * Tambien se resetean las propiedades de la clase.
 * @param[in]  ListEvent evento.
 */
void PatManagerWidget::OnItemDeselected(wxListEvent &ListEvent) {
   selectedPat_ = "";
   delete pNodeInfo_;
   pNodeInfo_ = NULL;
   ShowPatProperties();
   ListEvent.Skip();
}

/**
 * Este metodo responde a los eventos de activacion de algun item de la lista.
 * Permite abrir un PAT.
 * Un item es activado por un doble click o enter.
 * @param[in]  ListEvent evento.
 */
void PatManagerWidget::OnItemActivated(wxListEvent &ListEvent) {
   DoLoad();
}

/**
 * Este metodo responde a los eventos que suceden cuando el usuario presiona
 * alguna tecla.
 * Por ahora solo procesa la tecla "Suprimir" lo cual permite eliminar el PAT
 * seleccionado.
 * @param[in]  KeyEvent evento. Permite identificar la tecla presionada.
 */
void PatManagerWidget::OnKeyPress(wxKeyEvent &KeyEvent) {
   switch (KeyEvent.GetKeyCode()) {
      case WXK_DELETE:
         DeletePat();
         break;
      default:
         break;
   }
}

/**
 * Evento OnUIUpdate para habilitar o no los botones de la toolbar,
 * bajo ciertas condiciones.
 * La condicion bajo la cual los botones se activan es que haya al menos 1 item
 * de la lista activado y que no se este mostrando el Part PatInfoPart.
 * @param[in]  Event evento.
 */
void PatManagerWidget::OnUIUpdate(wxUpdateUIEvent &Event) {
   int item = USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl,
         GetSelectedItemCount(), 1);
   bool aux = false;
   if (item > 0 && !pPatInfo_)
      aux = true;
   // por que no anda esto??
   // Event.Enable(aux);
   // no deberia ser asi
   GET_CONTROL(*pToolWindow_, "ID_EDIT_PAT", wxButton)->Enable(aux);
   GET_CONTROL(*pToolWindow_, "ID_LOAD_PAT", wxButton)->Enable(aux);
   GET_CONTROL(*pToolWindow_, "ID_COMPRESS_PAT", wxButton)->Enable(aux);
   GET_CONTROL(*pToolWindow_, "ID_DELETE_PAT", wxButton)->Enable(aux);
}

/**
 * Este metodo responde al evento de cerrado de la ventana de edicion de prop.
 * del PAT.
 * Ejecuta el metodo PartContainerWidget::OnClose que contiene la logica para
 * verificar si hubo cambios en el part y preguntar si se desean salvar los cambios.
 * @param[in]  Event evento.
 */
void PatManagerWidget::OnClose(wxCloseEvent &Event) {
   PartContainerWidget::OnClose(Event);
}

/**
 * Devuelve la ventana padre donde se insertan los Part para editar las
 * propiedades del PAT y para visualizar la Informacion de Pat.
 * @return puntero a la ventan que contiene a dichos Part.
 */
wxWindow *PatManagerWidget::GetPartParent() {
   wxWindow *pwin = NULL;
   pwin = pToolWindow_->FindWindow("ID_CONTAINER_WIN");
   return pwin;
}

/**
 * Retorna un bitmap con el icono de la ventana.
 */
void PatManagerWidget::GetWindowIcon(wxBitmap& ToolBitmap, int X, int Y) const {
   GET_BITMAP_RESOURCE(wxT(icon_APPLICATION), ToolBitmap);
}

/**
 * Presenta un dialogo con la descripcion del PAT que se va a instalar y segun
 * el usuario confirme se instala o no el PAT.
 * @param[in]  UrlCommpressedPat url del PAT comprimido.
 * \note este metodo es invocado desde MainWindow::InstallCommpressedPat para
 * instalar un PAT cuando se inicia la aplicacion con un parametro.
 */
void PatManagerWidget::Install(const wxString &CommpressedPatUrl) {
   wxFileName fnpath(CommpressedPatUrl);
   PatFile patfile(
         wxString(
               fnpath.GetVolume().c_str()
                     + (fnpath.HasVolume() ? fnpath.GetVolumeSeparator() : wxT(""))
                     + fnpath.GetFullPath(wxPATH_UNIX)).c_str());

   //Creo el PAT y la ventana de Confirmacion.
   if (patfile.GetPatType() == PatFile::PatInvalid) {
      SHOW_ERROR(message_PAT_INSTALL_ERROR_DESCRIPTION_s,
                 patfile.GetErrorMsg().c_str());
      return;
   }

   // creo ventana de confirmacion.
   //tanto ppart como el PatPropertiesHtmlViewerPart se eliminan cuando se
   //cierra la ventana
   suri::PartContainerWidget *ppart = new suri::PartContainerWidget(
         new PatPropertiesHtmlViewerPart(CommpressedPatUrl.c_str()),
         wxT(caption_INSTALL_PAT),
         SUR_BTN_OK | SUR_BTN_CANCEL,
         SUR_BTN_OK | SUR_BTN_CANCEL);

   if (ppart->ShowModal() == wxID_OK) {
      std::string pathpat = MainWindow::Instance().GetPathRootPat()
            + patfile.GetPatName() + wxFileName::GetPathSeparator(wxPATH_UNIX);
      wxFileName fn(pathpat);
      if (fn.DirExists()) {
         if (SHOWQUERY(pToolWindow_, _(question_OVERWRITE_DATA),
               wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION) == wxID_NO)
            return;
      }

      if (!patfile.InstallPat(MainWindow::Instance().GetPathRootPat())) {
         SHOW_ERROR(message_PAT_INSTALL_ERROR_DESCRIPTION_s,
                    patfile.GetErrorMsg().c_str());
         //no se traduce, lo hace SHOW_
         //comento ya que DeleteFiles utiliza RemoveDir.
#ifdef __UNUSED_CODE__
         // Esto deberia hacerlo patfile en todo caso
         PatInfoPart *ppatinfo = new PatInfoPart(NULL, NULL, true);
         wxString errormsg;
         if(!ppatinfo->DeleteFiles(pathpat, errormsg))
         SHOW_ERROR(errormsg);

         delete ppatinfo;
#else
         if (!suri::RemoveDir(fn))
            SHOW_ERROR(_(message_DELETE_DIR_ERROR_s), fn.GetFullPath().c_str());
#endif
         return;
      }
      RefreshPatList();
      SHOW_INFO(message_PAT_INSTALLATION_SUCCESSFULL);
      //No lo traduce, lo hace SHOW_
   }
}

/**
 * Agrega un Part al espacio asignado en el PatManagerWidget.
 * @param[in]  pPart: Part a asignar.
 * @return true si puede agrega el Part.
 * @return false si no puede agrega el Part.
 */
bool PatManagerWidget::DoInsert(suri::Part *pPart) {
   if (AddControl(pPart, wxT("ID_CONTAINER_WIN"))) {
      EnableElements(pPatProp_ != NULL);
      return true;
   }
   return false;
}

/**
 * Elmina el Part pasado por parametro.
 * @param[in]  pPart Part a eliminar.
 * @return true en caso de haber eliminado el Part con exito.
 * @return false en caso de no poder eliminar el Part.
 */
bool PatManagerWidget::DoRemove(suri::Part *pPart) {
   delete pPart;
   return true;
}

/**
 * Genera los items de la lista de PATs. Asocia una imagen, de la lista de
 * imagenes de la lista, a cada PAT dependiendo de si es PAT de usuario o
 * propietario.
 * \pre Valida que el nombre del directorio de instalacion del PAT coincida con
 * el contenido del nodo informacion|nombre.
 * \post muestra el PAT en la lista.
 * @param[in]  Paths lista con la ruta de los archivos descripcion.xml
 * @param[in]  Path ruta del directorio de trabajo de los PATs de usuario o
 * propietario.
 */
void PatManagerWidget::AddItems(wxArrayString Paths, const std::string &Path) {
   for (size_t i = 0; i < Paths.GetCount(); i++) {
      wxFileName fnpath(Paths[i].c_str());
      wxString pathaux = fnpath.GetVolume().c_str()
            + (fnpath.HasVolume() ? fnpath.GetVolumeSeparator() : wxT(""));
      pathaux += fnpath.GetFullPath(wxPATH_UNIX);
      PatFile pat(pathaux.c_str());
      if (pat.GetPatType() != PatFile::PatInvalid) {
         /* si no coincide el nombre del directorio de instalacion del PAT con
          * el contenido del nodo informacion|nombre, no muestro el PAT en la
          * lista de PAT's instalados.
          */
         wxString patname = pat.GetPatName();
         if (patname.Cmp(fnpath.GetDirs().Item(fnpath.GetDirCount() - 1)) != 0) {
            REPORT_DEBUG("D:Nombre de directorio diferente a nodo informacion|nombre.");
            REPORT_DEBUG("D:%s ; %s", patname.c_str(),
                         fnpath.GetDirs().Item(fnpath.GetDirCount()-1).c_str());
            continue;
         }

         wxListItem item;
         size_t count = USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT",
               wxListCtrl, GetItemCount(), -1);
         int imageindex = 1;
         if (!IsUserPat(pathaux.c_str()))
            imageindex = 0;
         if (USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl,
               InsertItem(count, pat.GetPatName(), imageindex), -1) < 0)
            REPORT_DEBUG("D:No se pudo insertar %s.", pat.GetPatName().c_str());

         /* Seteo/Asocio un dato(imageindex) al elemento de la lista para luego
          * poder determinar si el item seleccionado es un PAT propietario o usr.
          */
         if (!USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl,
               SetItemData(count, imageindex), false))
            REPORT_DEBUG("D:No se pudo cargar el dato %s", "");
      }
   }
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->SetColumnWidth(
         0, wxLIST_AUTOSIZE);
}

void PatManagerWidget::LoadContextElementsLegacyVersion(
      PatFile& Pat, std::vector<std::string> &NotLoadedElements) {
   //Obtengo map de contextos y elementos.
   std::map<std::string, std::vector<wxXmlNode*> > contextmap = Pat.GetContextNodes();
   std::map<std::string, std::vector<wxXmlNode*> >::iterator itmap;
   for (itmap = contextmap.begin(); itmap != contextmap.end(); itmap++) {
      std::vector<wxXmlNode*> vecnodes;
      vecnodes.clear();
      vecnodes.insert(vecnodes.end(), itmap->second.begin(), itmap->second.end());
      std::vector<wxXmlNode*>::iterator it = vecnodes.begin();
      for (; it != vecnodes.end(); it++) {
         if (Pat.GetPatType() == PatFile::PatRelative) {
            //Cambio el contenido del nodo url. Hago absoluto el path del elemento.
            if (!GenericMakeUrlAbsolute(*it, selectedPat_)
                  || (Pat.GetDependencyNode(*it)
                        && !GenericMakeUrlAbsolute(Pat.GetDependencyNode(*it),
                                                   selectedPat_)))
               REPORT_DEBUG("D:Error al convertir URL a absoluto.");
         }
         //Ubico en lista de contextos, el contexto al que pertenece el elemento.
         std::list<Context*>::const_iterator contextit = pListContext_->begin();
         for (; contextit != pListContext_->end(); contextit++) {
            wxXmlNode *pproperties = (*contextit)->GetProperties();
            std::string contextname =
                  pproperties->GetPropVal(wxT(NAME_PROPERTY), "").c_str(); //No se traduce, elemento de XML
            delete pproperties;
            //Agrego el elemento al contexto correspondiente.
            if (contextname.compare(itmap->first) == 0) {
               if (!(*contextit)->AddElement(*it)) {
                  wxXmlNode *pchild = (*it)->GetChildren();
                  while (pchild && pchild->GetName().Cmp(wxT(URL_NODE)) != 0)
                     pchild = pchild->GetNext();

                  /* comento msg. de error y reuturn false para que se pueda
                   * cargar el PAT inclusive cuando hay elementos que no se
                   * pueden cargar. */
#ifdef __UNUSED_CODE__
                  //Muestra Error al insertar  + elemento + tipo de elemento +
                  //url del elemento + en contexto + nombre_del contexto
                  //SHOW_ERROR(message_INSERT_ELEMENT_IN_CONTEXT_ERROR_ss,
                  //             ((*it)->GetPropVal(wxT(TYPE_PROPERTY), "") + " " +
                  //             pchild->GetNodeContent()).c_str(),contextname.c_str());

                  SHOW_ERROR(message_INVALID_OR_DUPLICATED_CONTEXT_ELEMENT_ss,
                        (pchild->GetNodeContent()).c_str(), contextname.c_str());//no se traduce, lo hace SHOW_
                  return false;
#endif
                  //cargo el path de los elementos que no se pueden cargar.
                  NotLoadedElements.push_back(pchild->GetNodeContent().c_str());
               }
               CreateTempEmptyElement(*it);
               break;
            }
         }
      }
   }
}

void PatManagerWidget::LoadContextElements(
      const wxString& PatVersion, PatFile& Pat,
      std::vector<std::string> &NotLoadedElements) {
   if (PatVersion.StartsWith("1.2")) {
      LoadContextElementsLegacyVersion(Pat, NotLoadedElements);
   } else {
      wxXmlDocument docxml;
      wxXmlNode* pcontexts = Pat.GetContexts(&docxml);
      pcontexts = pcontexts->GetNext();

      //Obtengo map de contextos y elementos.
      std::map<std::string, std::vector<wxXmlNode*> > contextmap =
            Pat.GetContextNodes();
      std::map<std::string, std::vector<wxXmlNode*> >::iterator itmap;

      suri::ParameterCollection params;
      suri::ContextHandlerInterface* pxmlhandler =
            suri::ContextHandlerFactory::GetInstance()->Create(XMLMODE, params);

      /** Recorro todos los contextos, ya habiendo saltado el nodo informacion.*/
      while (pcontexts != NULL) {

         if (pcontexts->GetName().CompareTo(wxT(CONTEXT_NODE)) != 0) {
            pcontexts = pcontexts->GetNext();
            continue;
         }

         // Salteamos contextos vacios
         if (pcontexts->GetChildren() == NULL) {
            pcontexts = pcontexts->GetNext();
            continue;
         }
         // Si es relativo, buscamos la ruta previa
         std::string installpath = "";

         if (Pat.GetType() == PatFile::PatRelative) {
            installpath = suri::Configuration::GetParameter("pat_install_dir", wxT(""));
            wxXmlNode* pinfonode = Pat.GetInfoNode()->GetChildren();

            while (pinfonode->GetName().CompareTo(NAME_NODE) != 0)
               pinfonode = pinfonode->GetNext();

            installpath = installpath + pinfonode->GetNodeContent() + "/";
         }

         if (!pxmlhandler->Load(pcontexts, pDataViewManager_, installpath)) {
            REPORT_DEBUG("D:No se pudo cargar el xml del PAT");
//            return false;
         }

         pcontexts = pcontexts->GetNext();
      }
   }
}

/**
 * Carga los elementos de un pat en los contextos correspondientes.
 * \pre Verifica si hay un PAT cargado anteriormente.
 * \post Pregunta si se desea salvar sus cambios(en caso de que los
 * haya) y cierra el PAT.
 * @param[out] NotLoadedElements lista con path de elementos que no se cargaron.
 * @return true si pudo cargar los elemento en los contextos.
 * @return false si falla la carga de elementos.
 */
bool PatManagerWidget::LoadElements(std::vector<std::string> &NotLoadedElements) {
#if __RELOADPAT__
   if(PathXml.compare(openPat_)==0)
   REPORT_AND_FAIL_VALUE("D:Archivo ya abierto", true);
#endif
   std::auto_ptr<wxProgressDialog> pconfprog(
         new wxProgressDialog("Cargando PAT...", "", 6));
   pconfprog.get()->SetSize(wxSize(400, -1));
   pconfprog.get()->Centre(wxCENTRE_ON_SCREEN);
   //Limpio vector de elementos que no se pudieron cargar.
   NotLoadedElements.clear();
   pconfprog->Update(1);
   //Si hay un PAT cargado, salvo el contenido
   if (!MainWindow::Instance().GetPathPat().IsEmpty()) {
      PatInfoPart *ppatinfo = new PatInfoPart(
            MainWindow::Instance().GetInfoNode(), pListContext_,
            IsUserPat(MainWindow::Instance().GetPathPat()), pDataViewManager_);
      wxString xmldesc;
      if (!ppatinfo->CreatePat(xmldesc, MainWindow::Instance().GetInfoNode()))
         REPORT_DEBUG("D:Error al intentar obtener descripcion.");

      if (!xmldesc.IsEmpty()
            && xmldesc.Cmp(MainWindow::Instance().GetInitPatXml()) != 0) {
         if (SHOWQUERY(pToolWindow_, _(question_SAVE_CHANGES),
               wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION) == wxID_YES) {
            wxString pathaux =
                  wxFileName(MainWindow::Instance().GetPathPat().c_str()).GetPath(
                        wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
            ppatinfo->SaveXml(pathaux, xmldesc);
         }
      }
      delete ppatinfo;

      if (!MainWindow::Instance().CloseElements())
         return false;
   } else if (MainWindow::Instance().IsContextModified())
      MainWindow::Instance().ClosePat();
   pconfprog->Update(2);
   PatFile pat(selectedPat_);
   if (pat.GetPatType() == PatFile::PatInvalid)
      SHOW_ERROR("%s", false, pat.GetErrorMsg().c_str());
#if defined(CHECK_FIXED_PAT_VERSION) || defined(CHECK_APPLICATION_VS_PAT_VERSION)
   // Verifico la version de la aplicacion que creo el PAT contra la de la
   // aplicacion que lo esta cargando y si es menor, aborto la carga
   wxXmlNode *prootnode = pat.GetRootNode();
   if (!prootnode) {
      REPORT_DEBUG("D:No se pudo cargar el xml del PAT");
      return false;
   }
   wxString patapplicationversion = wxT("");
   prootnode->GetPropVal(wxT(APP_VERSION_PROPERTY), &patapplicationversion);
   delete prootnode;
   if (
#  ifdef CHECK_FIXED_PAT_VERSION
   patapplicationversion.Cmp(__PAT_INCOMPATIBLE_VERSION_STRING__)<=0
#  endif
#  if defined(CHECK_FIXED_PAT_VERSION) && defined(CHECK_APPLICATION_VS_PAT_VERSION)
   ||
#  endif
#  ifdef CHECK_APPLICATION_VS_PAT_VERSION
   patapplicationversion.Cmp(
         suri::Configuration::GetParameter("app_version", wxT("0.0"))) > 0
#  endif
         ) {
      SHOW_ERROR(message_INCOMPATIBLE_PAT_VERSION);
      return false;
   }
#endif
   pconfprog->Update(3);
   LoadContextElements(patapplicationversion, pat, NotLoadedElements);
   pconfprog->Update(4);

   //obtengo el nodo url del preview.
   if (pat.GetPatType() == PatFile::PatRelative) {
      wxXmlNode *pnodeaux = pNodeInfo_->GetChildren();
      while (pnodeaux && pnodeaux->GetName().Cmp(PREVIEW_NODE) != 0)
         pnodeaux = pnodeaux->GetNext();

      if (!MakeUrlAbsolute(pnodeaux, selectedPat_))
         REPORT_DEBUG("D:Error al convertir URL a absoluto.");
   }
   pconfprog->Update(5);

   //Guardo datos del PAT activo.
   MainWindow::Instance().SetInfoNode(pNodeInfo_);
   MainWindow::Instance().SetPathPat(selectedPat_);
   wxString xmldesc;
   if (!pPatInfo_) {
      pPatInfo_ = new PatInfoPart(pNodeInfo_, pListContext_, IsUserPat(selectedPat_),
                                  pDataViewManager_);
   }
   if (!pPatInfo_->CreatePat(xmldesc, pNodeInfo_))
      REPORT_AND_FAIL_VALUE("D:Error al cargar el PAT.", false);
   MainWindow::Instance().SetInitPatXml(xmldesc);

#ifdef __UNUSED_CODE__
   //si es un PAT de usuario modifico la variable app_hotlink_base_dir_volatile
   if(IsUserPat(selectedPat_))
   {

      wxFileName fn(selectedPat_);
      std::string hotlinkpath = fn.GetVolume().c_str();
      hotlinkpath += fn.HasVolume()?fn.GetVolumeSeparator():wxT("");
      hotlinkpath += fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR,
            wxPATH_UNIX).c_str();
      suri::Configuration::SetParameter("app_hotlink_base_dir_volatile",
            hotlinkpath);

   }
#endif

#if __RELOADPAT__
   openPat_ = PathXml;
#endif
   pconfprog->Update(6);
   return NotLoadedElements.empty();
}

/* Habilita/Deshabilita los botones del frame.
 * @param[in]  Enable puede ser true o false depende si se quiere Hab. o DesHab.
 */
void PatManagerWidget::EnableElements(const bool &Enable) {
   GET_CONTROL(*pToolWindow_, "ID_NEW_PAT", wxButton)->Enable(Enable);
   GET_CONTROL(*pToolWindow_, "ID_EDIT_PAT", wxButton)->Enable(Enable);
   GET_CONTROL(*pToolWindow_, "ID_LOAD_PAT", wxButton)->Enable(Enable);
   GET_CONTROL(*pToolWindow_, "ID_INSTALL_PAT", wxButton)->Enable(Enable);
   GET_CONTROL(*pToolWindow_, "ID_DELETE_PAT", wxButton)->Enable(Enable);
   GET_CONTROL(*pToolWindow_, "ID_COMPRESS_PAT", wxButton)->Enable(Enable);
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->Enable(Enable);
}

/**
 * Crea un PatPropertiesHtmlViewerPart para visualizar la informacion
 * del PAT seleccionado y lo carga en el espacio asignado por el PatManagerWidget.
 * @param[in]  PathPat path del archivo descripcion.xml.
 */
void PatManagerWidget::ShowPatProperties(const std::string &PathPat) {
   REPORT_DEBUG("D:Mostrando HTML");
   if (pPatInfo_ && !RemovePart(pPatInfo_))
      REPORT_DEBUG("D:Error al intentar eliminar la informacion de PAT.");
   pPatInfo_ = NULL;

   if (pPatProp_) {
      pPatProp_->SetHtml(PathPat);
      return;
   }

   // oculto los botones
   wxWindow *pwin = XRCCTRL(*pToolWindow_, wxT("ID_BUTTON_PANEL"),wxWindow);
   if (!pwin) {
      REPORT_DEBUG("D:No se pudo obtener el control %s", wxT("ID_BUTTON_PANEL"));
      pwin->Hide();
   }
   // Creo la parte que muestra las propiedades
   pPatProp_ = new PatPropertiesHtmlViewerPart(PathPat);
   if (!InsertPart(pPatProp_))
      REPORT_AND_FAIL("D:Error al intentar agregar PatProperties.");

   GET_CONTROL(*pToolWindow_, "ID_NEW_PAT", wxBitmapButton)->SetFocus();
}

/**
 * Crea un PatInfoPart para poder editar las propiedades del PAT y lo
 * carga en el espacio asignado por el PatManagerWidget.
 * @param[in]  IsEditing flag para indicar si se esta editando propiedades o si
 * se esta creando un PAT nuevo.
 */
void PatManagerWidget::ShowPatInfo(const bool &IsEditing) {
   REPORT_DEBUG("D:Mostrando Info");
   if (pPatProp_ && !RemovePart(pPatProp_))
      REPORT_DEBUG("D:Error al intentar eliminar Part.");

   pPatProp_ = NULL;
   if (pPatInfo_)
      return;
   // Muestro los botones
   wxWindow *pwin = XRCCTRL(*pToolWindow_, "ID_BUTTON_PANEL", wxWindow);
   if (!pwin)
      REPORT_AND_FAIL("D:No se pudo obtener el control %s", wxT("ID_BUTTON_PANEL"));
   pwin->GetParent()->GetSizer()->Show(pwin, true);
   pwin->GetParent()->GetSizer()->Layout();
   //oldname solo tiene contenido cuando se esta editando.
   wxString oldname = "";
   if (!selectedPat_.empty()) {
      PatFile pat(selectedPat_);
      oldname = pat.GetPatName();
   }

   pPatInfo_ = new PatInfoPart(pNodeInfo_, pListContext_, true, pDataViewManager_,
                               oldname, IsEditing);
   if (!InsertPart(pPatInfo_))
      REPORT_AND_FAIL("D:Error al intentar agregar PatInfo.");
}

/**
 * Verifica si es o no un PAT de usuario.
 * @param[in]  PathXml path absoluto del archivo descripcion.xml.
 * @return true en caso de que sea un PAT de usuario.
 * @return false en caso de que sea un PAT propietario.
 */
bool PatManagerWidget::IsUserPat(const wxString &PathXml) {
   if (PathXml.IsEmpty())
      return true;
   return PathXml.StartsWith(MainWindow::Instance().GetPathUserPat().c_str());
}

/**
 * Genera los Directorios de Trabajo de los Pat, uno para PAT de usuarios y uno
 * para PAT propietarios.
 * Los path de cada directorio los obtiene, a travez del MainWindow, del archivo
 * de configuracion de la aplicacion.
 */
void PatManagerWidget::CreatePatDirs() {
   wxFileName *pfn = new wxFileName(MainWindow::Instance().GetPathRootPat());
   if (pfn->IsDir()) {
      if (!pfn->Mkdir(MainWindow::Instance().GetPathRootPat(), 0755, wxPATH_MKDIR_FULL))
         REPORT_DEBUG("D:Error al crear directorio %s.", "");
   } else
      REPORT_DEBUG("D:Error al crear directorio %s.",
                   MainWindow::Instance().GetPathRootPat().c_str());
   delete pfn;

   pfn = new wxFileName(MainWindow::Instance().GetPathUserPat());
   if (pfn->IsDir()) {
      if (!pfn->Mkdir(MainWindow::Instance().GetPathUserPat(), 0755, wxPATH_MKDIR_FULL))
         REPORT_DEBUG("D:Error al crear directorio %s.", "");
   } else
      REPORT_DEBUG("D:Error al crear directorio %s.",
                   MainWindow::Instance().GetPathUserPat().c_str());
   delete pfn;
}

/**
 * Actualiza la lista de PATs.
 * Limpia y vuelve a cargar la lista de PATs, recorriendo los directorios de
 * trabajo de los PATs de usuario y propietarios .
 */
void PatManagerWidget::RefreshPatList() {
   //limpio y asigno la lista de imagenes a la lista.
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->ClearAll();
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->SetImageList(
         pImageList_, wxIMAGE_LIST_SMALL);
   wxListItem itemcol;
   itemcol.SetText(wxT("PAT")); //No se traduce
   GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->InsertColumn(0, itemcol);
   //Agrego los items.
   wxDir rootdir(MainWindow::Instance().GetPathRootPat());
   if (!rootdir.IsOpened())
      REPORT_AND_FAIL("D:Error buscando PATs instalados.");

   wxArrayString dirs;
   rootdir.GetAllFiles(MainWindow::Instance().GetPathRootPat(), &dirs,
                       "descripcion.xml");
   AddItems(dirs, MainWindow::Instance().GetPathRootPat());

   wxDir userdir(MainWindow::Instance().GetPathUserPat());
   if (!userdir.IsOpened())
      REPORT_AND_FAIL("D:Error buscando PATs instalados.");

   dirs.Empty();
   userdir.GetAllFiles(MainWindow::Instance().GetPathUserPat(), &dirs,
                       "descripcion.xml");
   AddItems(dirs, MainWindow::Instance().GetPathUserPat());

   PatFile pat(MainWindow::Instance().GetPathPat().c_str());
   if (pat.GetPatType() != PatFile::PatInvalid) {
      long itemidx = USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl,
            FindItem(-1, pat.GetPatName()), -1);
      GET_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT", wxListCtrl)->SetItemState(
            itemidx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      ShowPatProperties(MainWindow::Instance().GetPathPat().c_str());
   } else
      ShowPatProperties();
}

/**
 * Elimina el PAT seleccionado en la lista.
 * \pre Presenta un dialogo de confirmacion que debe ser aceptado
 * \post Elimina el directorio del PAT
 * \post Actualiza la lista de PATs.
 *
 * \todo Considerar renombrar a DeleteSelectedPat para mayor claridad
 */
void PatManagerWidget::DeletePat() {
   //verifico que haya al menos 1 seleccionado en la lista.
   if (selectedPat_.empty() || !USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT",
         wxListCtrl, GetSelectedItemCount(), 0)) {
      SHOW_ERROR(message_SELECT_PAT);
      //no se traduce, lo hace SHOW_
      return;
   }

   if (SHOWQUERY(pToolWindow_, _(question_CONFIRM_PAT_DELETION),
         wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION) != wxID_YES)
      return;

   //(PAT a desinstalar == PAT activo)? Cierro elem. : solo desinstalo.
   if (MainWindow::Instance().GetPathPat().Cmp(selectedPat_) == 0) {
      MainWindow::Instance().CloseElements();
      MainWindow::Instance().SetInfoNode(NULL);
      MainWindow::Instance().SetPathPat("");
   }

   PatInfoPart *ppatinfo = new PatInfoPart(NULL, NULL, true, pDataViewManager_);
   wxString errormsg;
   if (!ppatinfo->DeleteFiles(selectedPat_, errormsg))
      SHOW_ERROR(errormsg);
   delete ppatinfo;
   RefreshPatList();
}

/**
 * Crea un xml temporal con datos actualizados a partir del xml original.
 * \post Modifica el nodo fecha con la fecha actual
 * \post el xml con los datos modificados se salva en un archivo temporal
 *
 * @param[in]  PathPat: path absoluto del archivo descripcion.xml
 * @return el path absoluto del archivo temporal creado o cadena vacia en caso
 * de error.
 */
std::string PatManagerWidget::CreateXmlTemp(const std::string &PathPat) {
   if (PathPat.empty())
      return "";

   wxFileName fnpat(PathPat);
   wxXmlDocument doc;
   if (!doc.Load(wxT(fnpat.GetFullPath(wxPATH_UNIX)), suri::XmlElement::xmlEncoding_))
      return "";

   //obtengo el nodo informacion
   wxXmlNode *pnode = doc.GetRoot()->GetChildren();
   while (pnode && pnode->GetName().Cmp(wxT(INFORMATION_NODE)) != 0) //No se traduce, elemento XML
      pnode = pnode->GetNext();

   if (!pnode)
      return "";

   //obtengo el nodo fecha.
   wxXmlNode *pchild = pnode->GetChildren();
   while (pchild && pchild->GetName().Cmp(wxT(DATE_NODE)) != 0) //No se traduce, elemento XML
      pchild = pchild->GetNext();

   if (!pchild)
      return "";

   wxXmlNode *pnodeaux = pchild->GetChildren();
   if (!pnodeaux)
      return "";
   if (pnodeaux->GetName().Cmp(wxT(DAY_NODE)) != 0)
      return ""; //No se traduce, elemento XML

   pnodeaux->GetChildren()->SetContent(
         suri::LongToString(wxDateTime::Today().GetDay(), 2));
   pnodeaux = pnodeaux->GetNext();
   if (!pnodeaux)
      return "";
   if (pnodeaux->GetName().Cmp(wxT(MONTH_NODE)) != 0)
      return ""; //No se traduce, elemento XML
   /*TODO(21/10/2008 - Alejandro): Verificar valor de retorno de GetMonth() en
    * linux. En windows devuelve uno menos del mes actual.*/
   pnodeaux->GetChildren()->SetContent(
         suri::LongToString(wxDateTime::Today().GetMonth() + 1, 2));
   pnodeaux = pnodeaux->GetNext();
   if (!pnodeaux)
      return "";
   if (pnodeaux->GetName().Cmp(wxT(YEAR_NODE)) != 0)
      return ""; //No se traduce, elemento XML
   pnodeaux->GetChildren()->SetContent(
         suri::LongToString(wxDateTime::Today().GetYear()));
   wxString pathaux = fnpat.GetVolume().c_str()
         + (fnpat.HasVolume() ? fnpat.GetVolumeSeparator() : wxT(""));
   pathaux +=
         fnpat.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX).c_str();
   pathaux += fnpat.GetName() + "_.xml";
   wxString xmlstring;
   PatFile::SaveXml(xmlstring, doc.GetRoot());
   PatFile::SaveXml(pathaux, xmlstring);
   return pathaux.c_str();
}

/**
 * Verifica que coincida el xml con los archivos dentro del PAT.
 * @param[in]  PathPat path absoluto del PAT (comprimido).
 * @return true si el PAT es consistente
 * @return false si el PAT no es consistente.
 */
bool PatManagerWidget::VerifyCompressedPat(const std::string &PathPat) {
   //creo un archivo temporal para extraer el archivo descripcion.xml.
   wxFileName fntempfile(
         wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + "pat.xml");
   wxString aux = fntempfile.GetVolume().c_str()
         + (fntempfile.HasVolume() ? fntempfile.GetVolumeSeparator() : wxT(""))
         + fntempfile.GetFullPath(wxPATH_UNIX).c_str();
   std::string auxpath = wxFileName::CreateTempFileName(aux).c_str();

   if (auxpath.length() == 0)
      REPORT_AND_FAIL_VALUE("D:Falla creacion del archivo temporal.", false);

   //descomprimo el .xml en un archivo temporal
   suri::ZipFile zip(PathPat);
   if (!zip.Extract("descripcion.xml", auxpath, false, true))
      REPORT_AND_FAIL_VALUE("D:Error al descomprimir archivo descripcion.xml.", false);
   //verifico la consistencia del PAT.
   PatFile pat(PathPat);
   if (pat.GetPatType() == PatFile::PatInvalid)
      REPORT_AND_FAIL_VALUE("D:No se pudo verificar la creacion del PAT. Motivo: %s",
                            false, pat.GetErrorMsg().c_str());

   if (!pat.CheckConsistencyPat(auxpath, zip.ListFiles()))
      REPORT_AND_FAIL_VALUE("D:No se pudo verificar la creacion del PAT. Motivo: %s",
                            false, pat.GetErrorMsg().c_str());
   return true;
}

/**
 * Este metodo se utiliza como soporte para evitar colisiones de elementos
 * cuando se trabaja con elementos temporales como ser vectores dibujados,
 * hotlinks, etc.
 * Crea directorios temporales en la carpeta temporal de la App.
 * @param[in]  pElementNode nodo elemento.
 * @return true en caso de poder crear los directorios en el temp.: false C.O.C
 */
bool PatManagerWidget::CreateTempEmptyElement(const wxXmlNode *pElementNode) {
   if (!pElementNode || pElementNode->GetPropVal(wxT(TYPE_PROPERTY), wxT("")).Cmp(
   wxT(TYPE_PROPERTY_VALUE_VECTOR)) != 0)
      return false;

   //Obtengo el url.
   wxXmlNode *purlnode = pElementNode->GetChildren();
   GET_FIRST_CHILD(purlnode, URL_NODE);
   if (!purlnode)
      return false;

   std::string vectortempdir = suri::Configuration::GetParameter(
         "app_vector_base_dir_volatile", wxT("./"));
   if (!DoCreateTempEmptyElement(purlnode->GetContent(), vectortempdir))
      REPORT_AND_FAIL_VALUE("D: No se pudo crear directorio temporal para vector.",
                            false);

   //Verifico si el vector tiene HotLink asociado.
   wxXmlNode *purlhotlinknode = pElementNode->GetChildren();
   GET_FIRST_CHILD(purlhotlinknode, FEATURES_NODE);
   GET_FIRST_CHILD(purlhotlinknode, HTML_NODE);
   GET_FIRST_CHILD(purlhotlinknode, URL_NODE);
   if (!purlhotlinknode) //ya tengo el 1er nodo hijo del nodo url
      return true;

   std::string hotlinktempdir = suri::Configuration::GetParameter(
         "app_hotlink_base_dir_volatile", wxT("./"));
   bool retval = DoCreateTempEmptyElement(purlhotlinknode->GetContent(),
                                          hotlinktempdir);
   if (!retval)
      REPORT_DEBUG("D: No se pudo crear directorio temporal para hotlink.");
   return retval;
}

//! Genera directorios vacios
bool PatManagerWidget::DoCreateTempEmptyElement(const wxString &UrlElement,
                                                const wxString &DirTempPath) {
   if (UrlElement.IsEmpty() || DirTempPath.IsEmpty())
      return false;

   wxFileName fn(UrlElement);
   wxString tempdir = DirTempPath;
   if (fn.HasExt())
      tempdir += fn.GetDirs().Item(fn.GetDirCount() - 1).c_str();
   else
      tempdir += fn.GetName().c_str();

   tempdir += fn.GetPathSeparator();
   if (!fn.Mkdir(tempdir, 0755, wxPATH_MKDIR_FULL))
      return false;

   return true;
}

/**
 * Este metodo invoca a al metodo que carga un PAT en los contextos.
 * \post si la carga no falla, modifica el titulo de la ventana principal de la
 * aplicacion con el nombre del PAT cargado.
 * \post Cierra la ventana del Administrador de PATs.
 */
void PatManagerWidget::DoLoad() {
   if (selectedPat_.empty() || !USE_CONTROL(*pToolWindow_, "ID_LSTCTRL_PAT",
         wxListCtrl, GetSelectedItemCount(), 0)) {
      SHOW_ERROR(message_SELECT_PAT);
      //no se traduce, lo hace SHOW_
      return;
   }

   //El close se hace aca porque si cambias de ventana mientras se carga un
   //pat grande no se puede cerrar el PatManagerWidget.
//   pToolWindow_->GetParent()->Close();

   std::vector<std::string> notloadedelements;
   if (!LoadElements(notloadedelements)) {
#ifdef __SHOW_NOT_LOADED_FILES_TO_USER__
      if (!notloadedelements.empty()) {
         SHOW_ERROR(message_PAT_LOAD_ELEMENTS_ERROR);
         // Despliego barra de progreso
         std::vector<std::string>::iterator it = notloadedelements.begin();
         for (; it != notloadedelements.end(); it++)
            SHOW_WARNING("%s", (*it).c_str());
      }
#endif
      SHOW_ERROR(message_PAT_LOAD_ERROR);
      //no se traduce, lo hace SHOW_
   }

   PatFile pat(selectedPat_);
   if (pat.GetPatType() != PatFile::PatInvalid)
      MainWindow::Instance().SetWindowTitle(pat.GetPatName().c_str());

   EndModal(wxID_OK);
}
