/*! \file MainToolbar.cpp */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2014-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

// Includes standard
#include <string>
#include <list>

// Includes Suri
#include "MainToolbar.h"
#include "suri/LayerList.h"
#include "suri/xmlnames.h"
#include "suri/WxsLayerConfigurationPart.h"
#include "suri/PartContainerWidget.h"
#include "suri/DataViewManager.h"
#include "suri/WorkGroupInterface.h"
#include "suri/MutableWorkGroupInterface.h"
#include "suri/NodePath.h"
#include "suri/Element.h"
#include "suri/TreeNodeInterface.h"
#include "suri/AppConfigurationWidget.h"
#include "suri/Configuration.h"
#include "SeymurConfigurationPart.h"
#include "suri/messages.h"
#include "suri/Option.h"
#include "suri/LibraryToolBuilder.h"
#include "WxsLayerImporter.h"
// Includes Wx
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"

// Includes App
#include "messages.h"
#include "Context.h"
#include "MainWindow.h"

// Defines

// Forwards
// Forward para no tener que crear un .h
void InitPatToolbarXmlResource();

START_IMPLEMENT_EVENT_OBJECT(MainToolbarEventHandler,MainToolbar)
IMPLEMENT_EVENT_CALLBACK(OnManagePatButtonClick,OnManagePatButtonClick(event),wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnNewPatButtonClick,OnNewPatButtonClick(event),wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnAddElementButtonClick,OnAddElementButtonClick(event),wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnAddUrlElementButton, OnAddUrlElementButton(), wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnAddWmsElementButton, OnAddWmsElementButton(), wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnGroupElementsButton, OnGroupElementsButton(), wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnDeleteElementButtonClick,OnDeleteElementButtonClick(event),wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnSavePatButtonClick,OnSavePatButtonClick(event),wxCommandEvent)
//#ifdef __ENABLE_CONFIG_BUTTON__
IMPLEMENT_EVENT_CALLBACK(OnConfigureButtonClick,OnConfigureButtonClick(event),wxCommandEvent)
//#endif	// __ENABLE_CONFIG_BUTTON__
IMPLEMENT_EVENT_CALLBACK(UIUpdate,OnUIUpdate(event),wxUpdateUIEvent)
END_IMPLEMENT_EVENT_OBJECT

/**
 * Constructor.
 * @param[in]	pParent: puntero a la ventana padre.
 * @param[in]	pContext: contexto asociado.
 * @param[in]	pList: Lista de elementos asociada.
 */
MainToolbar::MainToolbar(wxWindow *pParent, Context *pContext,
                         suri::DataViewManager* pDataViewManager) :
      Widget(pParent), NEW_EVENT_OBJECT(MainToolbarEventHandler),
      pContext_(pContext), pDataViewManager_(pDataViewManager) {
   windowTitle_ = _(caption_PAT_TOOLBAR);
   CreateTool();
}

/**
 * Destructor.
 */
MainToolbar::~MainToolbar() {
   DELETE_EVENT_OBJECT;
}

/**
 * Carga la ventana diseniada. Si tuvo exito asocia los eventos con los metodos
 * @return true en caso de poder cargar la ventan. false C.O.C.
 */
bool MainToolbar::CreateToolWindow() {
   pToolWindow_ = wxXmlResource::Get()->LoadPanel(pParentWindow_, wxT("ID_PATTOOLBAR"));
   // Le cargo los metodos para los botones
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_NEW"),wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnNewPatButtonClick), NULL,
         pEventHandler_);
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_OPEN"),wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnManagePatButtonClick), NULL,
         pEventHandler_);
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_INSERT"),wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnAddElementButtonClick), NULL,
         pEventHandler_);
   XRCCTRL(*pToolWindow_, wxT("ID_ADD_URL_ELEMENT_BUTTON"), wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnAddUrlElementButton), NULL,
         pEventHandler_);
   XRCCTRL(*pToolWindow_, wxT("ID_BUTTON_ADD_WMS_ELEMENT"), wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnAddWmsElementButton), NULL,
         pEventHandler_);

   XRCCTRL(*pToolWindow_, wxT("ID_BUTTON_GROUPELEMENTS"), wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnGroupElementsButton), NULL,
         pEventHandler_);
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_DELETE"),wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnDeleteElementButtonClick),
         NULL, pEventHandler_);
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_SAVE"),wxBitmapButton)->Connect(
         wxEVT_COMMAND_BUTTON_CLICKED,
         wxCommandEventHandler(MainToolbarEventHandler::OnSavePatButtonClick), NULL,
         pEventHandler_);
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_SAVE"),wxBitmapButton)->Connect(
         wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainToolbarEventHandler::UIUpdate),
         NULL, pEventHandler_);
//XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_CONFIG"),wxBitmapButton)->Hide();
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_CONFIG"),wxBitmapButton)->
//#ifdef __ENABLE_CONFIG_BUTTON__
         Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                 wxCommandEventHandler(MainToolbarEventHandler::OnConfigureButtonClick),
                 NULL, pEventHandler_);
//#else
//   Hide();
//   pToolWindow_->Fit();
//#endif	// __ENABLE_CONFIG_BUTTON__

   pDeletedCb_ = new DeletedElement_Callback(this);
   suri::Event::GetInstance()->On("SEVT_DELETED_ELEMENT", pDeletedCb_);

   // cargo el tamanio conseguido como el deseado
   pToolWindow_->GetSize(&wantedWidth_, &wantedHeight_);

   return true;
}

/**
 *
 */
void MainToolbar::DeleteElement() {
   wxCommandEvent dummy;
   OnDeleteElementButtonClick(dummy);
}

/**
 * Este metodo se ejecuta cuando se produce un evento de click sobre el boton de
 * "Administrador de PATs"
 * @param[in]	Event: evento
 */
void MainToolbar::OnManagePatButtonClick(wxCommandEvent &Event) {
   MainWindow::Instance().OpenPat();
}

/**
 * Este metodo se ejecuta cuando se produce un evento de click sobre el boton de
 * "Agregar elemento". Presenta un dialogo de apertura de archivo.
 * Emite mensaje de error si fallo la apertura del archivo.
 * @param[in]	Event: evento
 */
void MainToolbar::OnAddElementButtonClick(wxCommandEvent &Event) {
   wxFileDialog dlg(pToolWindow_, _(caption_SELECT_ELEMENT), wxT(""), wxT(""),
                    pContext_->GetSupportedFilesExt());
   if (dlg.ShowModal() == wxID_OK) {
      // valida y agrega el elemento a la lista
      if (!pContext_->InsertElement(dlg.GetPath())) {
         SHOW_ERROR(
               message_INVALID_OR_DUPLICATED_CONTEXT_ELEMENT_ss,
               ((dlg.GetPath()).AfterLast('\\')).c_str(),
         pContext_->GetProperties()-> GetPropVal(wxT(NAME_PROPERTY), "").c_str());
      }
   }
}

/**
 * Evento que se dispara al presionar el boton Agregar Elemento URL
 * Despliega un control de tipo texto en donde se ingresa la url que se desea agregar
 * Una vez finalizada la operacion agrega el elemento url al pat
 */
void MainToolbar::OnAddUrlElementButton() {
   wxDialog dialog(NULL, wxID_ANY, _("Ingrese URL"), wxDefaultPosition,
                   wxDefaultSize, wxCAPTION | wxSYSTEM_MENU);
   dialog.SetSizer(new wxBoxSizer(wxVERTICAL));
   wxTextCtrl *ptxtctrl = new wxTextCtrl(&dialog, wxID_ANY,
                                         _("http://www.ejemplo.com.ar"),
                                         wxDefaultPosition, wxSize(250, -1));
   dialog.GetSizer()->Add(ptxtctrl, 0, wxEXPAND | wxALL, 5);
   dialog.GetSizer()->Add(dialog.CreateButtonSizer(wxOK), 0, wxEXPAND | wxALIGN_BOTTOM);
   dialog.Fit();
   wxString newgroupname;
   ptxtctrl->SetValidator(wxTextValidator(wxFILTER_NONE, &newgroupname));
   ptxtctrl->SelectAll();
   ptxtctrl->SetFocusFromKbd();

   // muestro el dialogo
   if (dialog.ShowModal() != wxID_OK)
      return ;

   if (newgroupname.empty())
      SHOW_ERROR("No es posible agregar URL %s", newgroupname.c_str());
   // valida y agrega el elemento a la lista
   if (!pContext_->InsertElement(newgroupname)) {
         SHOW_ERROR(
               message_INVALID_OR_DUPLICATED_CONTEXT_ELEMENT_ss,
               newgroupname.c_str(),
         pContext_->GetProperties()->GetPropVal(wxT(NAME_PROPERTY), "").c_str());
   }

}

/**
 * Evento que se dispara al presionar el boton Agregar Elemento WMS.
 * Despliega dialogo para cargar datos de fuente wms.
 * Una vez finalizada la operacion agrega el elemento wms al pat
 */
void MainToolbar::OnAddWmsElementButton() {
   std::string factorystring;
   suri::Option metadata;
   suri::WxsLayerImporter* pimportwidget = new suri::WxsLayerImporter(pDataViewManager_,
                                                          factorystring,
                                                          metadata);
   if (pimportwidget->CreateTool() && pimportwidget->ShowModal() == wxID_OK) {
      // se comenta hasta que este terminada la implementacion
      suri::DatasourceManagerInterface* dm = pDataViewManager_->GetDatasourceManager();
      const char* pdatasourcetypes[] = { "RasterDatasource", "VectorDatasource" };
      suri::DatasourceInterface* pdatasource = NULL;
      for (int i = 0; i < 2 && pdatasource == NULL; ++i)
         pdatasource = suri::DatasourceInterface::Create(pdatasourcetypes[i],
                                                         factorystring, metadata);
      if (pdatasource != NULL && !dm->AddDatasource(pdatasource))
      SHOW_ERROR(message_ADD_ELEMENT_ERROR);
   }
}

/** Agrupa los elementos seleccionados */


void MainToolbar::OnGroupElementsButton() {
   // creo un dialogo para ingresar un txt
   std::string newgroupname;

   if (!GetGroupName(newgroupname))
      return;

   // Agrego grupo al final del arbol
   suri::WorkGroupInterface* prootworkgroup =
         pDataViewManager_->GetGroupManager()->GetRootWorkGroup();
   suri::NodePath rootpath = prootworkgroup->GetRootPath();
   if( !rootpath.IsValid( ))
	   return;
   suri::NodePath grouppath =
			pDataViewManager_->GetGroupManager()->InsertContextGroup(
					pDataViewManager_->GetViewcontextManager()->GetSelectedViewcontextId(),
					newgroupname.c_str(), rootpath,
					prootworkgroup->GetRootSize(), pContext_->GetViewcontextType());

   std::list<suri::NodePath> selectednodes = pDataViewManager_->
                           GetSelectionManager()->GetSelectedNodes();
   std::list<suri::NodePath>::iterator it = selectednodes.begin();

   // Si el grupo fue creado con una imagen seleccionada, esta imagen
   // pasa a ser parte del grupo.
   for (;it != selectednodes.end(); ++it)
      if (!it->AllowsChildren())
         pDataViewManager_->GetGroupManager()->MoveNode(*it, grouppath,0);
   delete prootworkgroup;
}

/**
 * Muestra pantalla para que el usuario ingrese un nombre de grupo.
 * @param[out] GroupName nombre del grupo ingresado por el usuario
 * @return bool que que indica si tuvo exito.
 */
bool MainToolbar::GetGroupName(std::string &GroupName) const {
   wxDialog dialog(NULL, wxID_ANY, _(caption_NEW_GROUP_INPUT), wxDefaultPosition,
                   wxDefaultSize, wxCAPTION | wxSYSTEM_MENU);
   dialog.SetSizer(new wxBoxSizer(wxVERTICAL));
   wxTextCtrl *ptxtctrl = new wxTextCtrl(&dialog, wxID_ANY,
                                         _(content_DEFAULT_GROUP_NAME),
                                         wxDefaultPosition, wxSize(250, -1));
   dialog.GetSizer()->Add(ptxtctrl, 0, wxEXPAND | wxALL, 5);
   dialog.GetSizer()->Add(dialog.CreateButtonSizer(wxOK), 0, wxEXPAND | wxALIGN_BOTTOM);
   dialog.Fit();

   // Valor por default
   wxString newgroupname = _(content_DEFAULT_GROUP_NAME);
   ptxtctrl->SetValidator(wxTextValidator(wxFILTER_NONE, &newgroupname));
   ptxtctrl->SelectAll();
   ptxtctrl->SetFocusFromKbd();

   // muestro el dialogo
   if (dialog.ShowModal() != wxID_OK)
      return false;

   GroupName = newgroupname.c_str();

   return true;
}


/**
 * Este metodo se ejecuta cuando se produce un evento de click sobre el boton de
 * "Nuevo PAT". Valida que haya elementos en los contextos o que haya un PAT
 * cargado.
 * @param[in]	Event: evento
 */
void MainToolbar::OnNewPatButtonClick(wxCommandEvent &Event) {
   /* Si tengo un PAT cargado o se modificaron los contextos =>
    * 														permito ejecutar "Nuevo PAT". */
   if (!MainWindow::Instance().GetPathPat().IsEmpty()
         || MainWindow::Instance().IsContextModified())
      MainWindow::Instance().ClosePat();
}

/**
 * Este metodo se ejecuta cuando se produce un evento de click sobre el boton de
 * "Cerrar elemento."
 * @param[in]	Event: evento
 */
void MainToolbar::OnDeleteElementButtonClick(wxCommandEvent &Event) {
   if (SHOWQUERY(GetWindow(), _(message_ELEMENT_REMOVE_ELEMENT_CONFIRMATION),
                      wxYES|wxNO|wxICON_QUESTION) == wxID_NO)
      return;

   // Elmino capas seleccionadas
   std::list<suri::NodePath> selectednodes =
         pDataViewManager_->GetSelectionManager()->GetSelectedNodes();
   std::list<suri::NodePath>::iterator it = selectednodes.begin();
   bool removeresult = true;
   for (; it != selectednodes.end() && removeresult; ++it)
      if (!it->AllowsChildren()) {
         suri::ViewcontextInterface* pviewcontext =
               pDataViewManager_->GetViewcontextManager()->GetSelectedViewcontext();
         removeresult = false;
         if (pviewcontext) {
            suri::LayerInterface* player = pviewcontext->GetAssociatedLayer(
                  it->GetLastPathNode()->GetContent());
            if (player) {
               removeresult = pviewcontext->RemoveLayer(player->GetId());
               // Remuevo del arbol la fuente de datos a eliminar.
               suri::TreeNodeInterface* dsnode = it->GetLastPathNode();
               suri::MutableWorkGroupInterface* prootworkgroup =dynamic_cast<suri::MutableWorkGroupInterface *>
                     (pDataViewManager_->GetGroupManager()->GetRootWorkGroup());
               std::string dsid = dsnode->GetContent();
               suri::NodePath npath = prootworkgroup->SearchContent(dsid);
               if ( npath.IsValid()) {
                  prootworkgroup->Remove(npath);
               }
            }
         }
      }
   if (!removeresult) {
      SHOW_ERROR(_(message_LAYER_BLOCK));
      return;
   }

   // Elimino grupos seleccionados
   selectednodes = pDataViewManager_->GetSelectionManager()->GetSelectedNodes();
   removeresult = true;
   it = selectednodes.begin();
   for (; it != selectednodes.end() && removeresult; ++it)
      if (it->AllowsChildren())
         removeresult = pDataViewManager_->GetGroupManager()->RemoveGroup(*it);
   if (!removeresult) {
      SHOW_ERROR("No se puede elminar el grupo");
      return;
   }
   pContext_->ForceListUpdate();
   pContext_->ForceViewerUpdate();
}

/**
 * Este metodo se ejecuta cuando se produce un evento de click sobre el boton de
 * "Guardar PATs"
 * @param[in]	Event: evento
 */
void MainToolbar::OnSavePatButtonClick(wxCommandEvent &Event) {
   MainWindow::Instance().SavePat();
}

//#ifdef __ENABLE_CONFIG_BUTTON__
//! Evento de los botones: Configurar
void MainToolbar::OnConfigureButtonClick(wxCommandEvent &Event) {
   std::string appname = suri::Configuration::GetParameter("app_name", "");

   suri::AppConfigurationWidget* pappconf =
            new suri::AppConfigurationWidget(new SeymurConfigurationPart(pDataViewManager_),
                                             wxT("Software 2MP, ventana de configuracion"),
                                             pDataViewManager_);
   /** Insertando los parts en el partcontainer*/
   pappconf->InsertParts();
   /** Muestro la ventana ya inicializada*/
   pappconf->ShowModal();
   return;
}
//#endif	// __ENABLE_CONFIG_BUTTON__
/**
 * Este metodo se ejecuta cuando se produce un evento de UIUpdate. Su objetivo
 * es habilitar o no el boton de "Guardar PAT" en base a si fue o no modificado
 * el contexto.
 * @param[in]	Event: evento
 */
void MainToolbar::OnUIUpdate(wxUpdateUIEvent &Event) {
   // Si la barra de herramientas esta deshabilitada no tengo que hacer nada.
   if (!pToolWindow_->IsEnabled()) return;
   //Solo (des)habilita el toolbar si esta en estado contrario
   bool enabled =
         XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_SAVE"),wxBitmapButton)->IsEnabled();
   bool contextmodified = MainWindow::Instance().IsContextModified();
   if (enabled != contextmodified)
      XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_SAVE"),wxBitmapButton)->Enable(
            contextmodified);
   bool not2dcontext = (pContext_->GetContextType() != Context::Context2DType) &&
                        (pContext_->GetContextType() != Context::ContextMapType);
   XRCCTRL(*pToolWindow_,wxT("ID_BUTTON_ADD_WMS_ELEMENT"), wxBitmapButton)->Enable(!not2dcontext);
   XRCCTRL(*pToolWindow_,wxT("ID_ADD_URL_ELEMENT_BUTTON"), 
                     wxBitmapButton)->Enable(not2dcontext &&
                     pContext_->GetContextType() != Context::Context3DType);
   Event.Skip();
}

/***************************************************/

/**
 * Ctor.
 */
DeletedElement_Callback::DeletedElement_Callback(MainToolbar* pToolbar)
   : pToolbar_(pToolbar) {

}

/** Metodo a reimplementar en cada evento */
void DeletedElement_Callback::Handle(suri::BaseParameter* pParam) {
   if (pToolbar_ != NULL)
      pToolbar_->DeleteElement();
}
