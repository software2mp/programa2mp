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

//Includes Suri
#include "suri/LayerList.h"
#include "suri/ViewerWidget.h"
#include "suri/Configuration.h"
#include "suri/xmlnames.h"
#include "suri/AbstractFactory.h"
#include "suri/LayerListWidget.h"
#include "suri/Tool.h"
#include "DefaultViewcontext.h"
#include "suri/HtmlTreeWidget.h"
#include "suri/LayerTreeModel.h"
#include "suri/LayerHtmlTreeNodeHandler.h"
#include "DefaultViewcontextManager.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"
#include "wx/splitter.h"
#include "wx/choicebk.h"
#include "wx/notebook.h"

//Includes App
#include "messages.h"
#include "MainToolbar.h"
#include "Context.h"
#include "ApplicationWindowManager.h"

//Defines

HAS_FACTORY_IMPLEMENTATION_COMMON(Context, 1, DeclareParameters_1(wxWindow*))

// Forward para no tener que crear un .h
void InitGenericContextPanelXmlResource();
void InitContextToolsXmlResource();

START_IMPLEMENT_EVENT_OBJECT(ContextEvent,Context)IMPLEMENT_EVENT_CALLBACK(OnButtonHelp,OnButtonHelp(),wxCommandEvent)
IMPLEMENT_EVENT_CALLBACK(OnToolSelecting,OnToolSelecting(event),wxNotebookEvent)
IMPLEMENT_EVENT_CALLBACK(OnToolSelected,OnToolSelected(event),wxNotebookEvent)
IMPLEMENT_EVENT_CALLBACK(OnUIUpdate, OnUIUpdate(event), wxUpdateUIEvent)
END_IMPLEMENT_EVENT_OBJECT

/**
 * Ctor.
 * @param[in] pParent puntero a ventana padre.
 * @param[in] pDataViewManager puntero a visualizador del contexto.
 * @param[in] ContextType tipo de contexto para el XML de XmlElement.
 */
Context::Context(wxWindow *pParent, suri::DataViewManager* pDataViewManager,
                 const wxString &ContextType) :
      Widget(pParent), XmlElement(wxT(CONTEXT_NODE),  //No se traduce, elemento XML
            wxT(ContextType)),
      pElementList_(new suri::LayerList), pViewer_(NULL), pDataViewManager_(pDataViewManager),
      typeContext_(ContextGenericType), supportedFilesExt_(wxT("")),
      pTreeWidget_(NULL) , currentTool_(-2),
      pCurrentTool_(NULL), NEW_EVENT_OBJECT(ContextEvent),
      preventAssert_(true), ptoolbook_(NULL), currentChoicebookSelection_(-1),
      isChangingTool_(false), pLayersTree_(NULL) {
   viewcontextId_ = suri::SuriObject::NullUuid;
}

/**
 * Dtor.
 * Elmina objeto de eventos, herramientas y lista de elementos.
 */
Context::~Context() {
   DELETE_EVENT_OBJECT;
//   std::multimap<int, suri::Tool*>::iterator it = toolMap_.begin();
//   while (it != toolMap_.end()) {
//      suri::Tool* ptool = (*it).second;
//      delete ptool;
//      it++;
//   }

   for (int ix = 0, lenix = canDelTools_.size(); ix < lenix; ++ix)
      delete canDelTools_[ix];

   canDelTools_.clear();

   delete pElementList_;
}

/**
 * Crea (carga) una ventana base de los contextos, contiene :
 * - Control grafico para la lista de elementos.
 * - Toolbar de herramientas comunes a todos los Contextos.
 * - 2 espacios para insertar toolbars
 * - Espacio para 1 visualizador
 * @return true.
 */
bool Context::CreateToolWindow() {
	pTreeWidget_ = NULL;
	pToolWindow_ = wxXmlResource::Get()->LoadPanel(pParentWindow_,
			wxT("ID_GENERICCONTEXT"));
	// configuracion del splitter
	XRCCTRL(*pToolWindow_, wxT("ID_SPLITTERWINDOW"), wxSplitterWindow)->SetMinimumPaneSize(
			200);
	XRCCTRL(*pToolWindow_, wxT("ID_SPLITTERWINDOW"), wxSplitterWindow)->UpdateSize();

	// Agrego la lista de capas
#ifdef __USE_LAYER_TREE_WIDGET__
	suri::LayerTreeWidget* players = new suri::LayerTreeWidget(
			XRCCTRL(*pToolWindow_, wxT("ID_SPLITTERWINDOW"), wxSplitterWindow),
			pElementList_);
#else    // __USE_LAYER_TREE_WIDGET__
	// Construccion de la parte comun de los Contextos
	suri::ViewcontextInterface::ViewcontextType type = GetViewcontextType();
	pViewcontext_ = new suri::DefaultViewcontext(false, true, type);
	pViewcontext_->SetLayerList(pElementList_);
	InitializeLayerListWidget();
#endif   // __USE_LAYER_TREE_WIDGET__

	// La barra de herramientas generica de los contextos
	MainToolbar *ppattoolbar = new MainToolbar(pToolWindow_, this,
			pDataViewManager_);
	AddControl(ppattoolbar, wxT("ID_CONATINERLEFT"));

	XRCCTRL(*pToolWindow_, wxT("ID_CONTEXTHELPBUTTON"), wxBitmapButton)->Connect(
			wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(ContextEvent::OnButtonHelp), NULL,
			pEventHandler_);
	return true;
}

/** Metodo auxiliar que inicializa la lista de capas correspondiente
 *  al visualizador activo (en caso de Simur es unico por contexto)
 */
void Context::InitializeLayerListWidget() {
   /**
   suri::LayerListWidget *players = new suri::LayerListWidget(
     XRCCTRL(*pToolWindow_,wxT("ID_SPLITTERWINDOW"),wxSplitterWindow), pViewcontext_,
     pElementList_, pDataViewManager_);
     **/
   if (pTreeWidget_ == NULL)  {
      wxWindow *playerparent = XRCCTRL(*pToolWindow_, wxT("ID_LAYERS"), wxWindow);
      suri::LayerTreeModel* pguitreemodel = new suri::LayerTreeModel(
                           pDataViewManager_->GetGroupManager(),
                           pDataViewManager_->GetSelectionManager());
      suri::LayerHtmlTreeNodeHandler* playerhtmltreenode = new suri::LayerHtmlTreeNodeHandler(
                           pDataViewManager_,
                           pDataViewManager_->GetGroupManager(),
                           pguitreemodel, suri::LayerHtmlTreeNodeHandler::CheckableHtmlNode
                                          | suri::LayerHtmlTreeNodeHandler::ContextMenu);
      pTreeWidget_ = new suri::HtmlTreeWidget(
                     playerparent,
                     pguitreemodel,
                     playerhtmltreenode,
                     pDataViewManager_->GetGroupManager(),
                     suri::HtmlTreeWidget::MultipleSelection |
                     suri::HtmlTreeWidget::DraggableItems);
      AddControl(pTreeWidget_, wxT("ID_LAYERS"));
   }
}
/**
 * Agrega una herramienta al contexto.
 *
 * Agrega herramientas al contexto.
 * Si la herramienta posee una ventana asociada GetToolWindow()!=NULL,
 * agrega dicha ventana al control grafico de herramientas
 *
 * El control grafico de herramientas se inicializa en la primer llamada
 * con una herramienta con ventana asociada.
 *
 *
 * \todo se podria generalizar el wxChoicebook a un wxBaseBook
 * @param[in] pTool Herramienta a agregar
 * @return true en caso de poder agregar la herramienta.
 * @return false pTool invalido
 * \post La ventana asociada a la herramienta se inserta en el control
 *       grafico.
 * \post La primer herramienta agregada es activada.
 * \post Si la herramienta no tiene ventana se activa siempre
 * \post Las nuevas ventanas se desactivan.
 * \post Se conecta el evento UiUpdate para habilitar la ventana asociada
 *       en funcion del estado de activacion de la herramienta.
 */
bool Context::AddTool(suri::Tool *pTool) {
   if (!pTool)
   REPORT_AND_RETURN_VALUE("D:Herramienta invalida.", false);
   if (!preventAssert_) {
      ptoolbook_ = XRCCTRL(*pToolWindow_,wxT("ID_CONTEXTTOOL_CHOICE"),
            wxChoicebook);
   }
   pTool->SetList(pElementList_);
   pTool->SetViewer(pViewer_);
   wxWindow *ptoolwindow = pTool->GetToolWindow();
   wxString toolname = pTool->GetToolName();
   REPORT_DEBUG("D:Agregando herramienta %s", toolname.c_str());
   bool retval = false;
   int idx = -1;
   if (ptoolwindow) {
      // Si no existe el panel, es la primer herramienta
      if (!ptoolbook_) {
         // obtengo el panel del recurso
         wxPanel *ppanel = wxXmlResource::Get()->LoadPanel(pToolWindow_,
                                                           wxT("ID_CONTEXTTOOLS"));

         ptoolbook_ = XRCCTRL(*ppanel,wxT("ID_CONTEXTTOOL_CHOICE"),wxChoicebook);
         if (!ptoolbook_)
        	 REPORT_AND_FAIL_VALUE("D:Toolbook invalido", false);
         // conecto los eventos de cambio de pagina
         ptoolbook_->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
                             wxNotebookEventHandler(ContextEvent::OnToolSelecting),
                             NULL, pEventHandler_);
         ptoolbook_->Connect(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,
                             wxNotebookEventHandler(ContextEvent::OnToolSelected), NULL,
                             pEventHandler_);
         retval = AddControl(ppanel,
                             XRCCTRL(*pToolWindow_,wxT("ID_CONATINERRIGHT"),wxWindow),
                             suri::ControlArrangement(0, 0));
         if (retval)
        	 preventAssert_ = false;

         // primera herramienta cargada como la actual
         pCurrentTool_ = pTool;
         // La primer herramienta va siempre activa
         pTool->SetActive();
         // asumo que tiene idx = 0 (el pulldown no tiene paneles agregados por
         // fuera de esta rutina)
         currentTool_ = 0;
      } else {
         retval = true;
         // El resto de las herramientas va inactivas
         pTool->SetActive(false);
      }
      // Agrego un panel al choice
      wxPanel *ptemppanel = new wxPanel(ptoolbook_, wxID_ANY);
      size_t count = ptoolbook_->GetPageCount();
      retval &= ptoolbook_->InsertPage(count, ptemppanel, toolname);
      // Hago que agregue como un Widget usando el panel agregado como parent
      AddControl(dynamic_cast<Widget*>(pTool), ptemppanel);
      wxSize size = ptoolbook_->GetSize();
      REPORT_DEBUG("D:%s : (%d ; %d)",
                   pTool->GetToolName().c_str(), size.GetWidth(), size.GetHeight());

#if __UNUSED_CODE__
      //Agregado para que el codigo siguiente no se ejecute y no perderlo
      // Permite agrandar el pulldown
      ptoolbook_->Layout();
      ptoolbook_->GetSizer()->Layout();
      ptoolbook_->GetSizer()->GetItem((size_t)(0))->GetSizer()->Layout();
      ptoolbook_->GetSizer()->GetItem((size_t)(0))->GetSizer()->
      GetItem((size_t)(0))->GetWindow()->SetMinSize(wxSize(200,-1));
      ptoolbook_->GetSizer()->GetItem((size_t)(0))->GetSizer()->
      GetItem((size_t)(0))->GetWindow()->Layout();
#endif

      idx = ptoolbook_->GetPageCount() - 1;
      ptoolwindow->Connect(wxEVT_UPDATE_UI,
                           wxUpdateUIEventHandler(ContextEvent::OnUIUpdate), NULL,
                           pEventHandler_);
   } else {
      // Si no tiene ventana, va siempre activa
      pTool->SetActive();
      retval = true;
   }

   if (dynamic_cast<Widget*>(pTool) == NULL)
      canDelTools_.push_back(pTool);

   toolMap_.insert(std::make_pair(idx, pTool));

#ifdef __DEBUG__
   if (retval)
      REPORT_DEBUG("D:Herramienta %s agregada con exito", toolname.c_str());
   else
      REPORT_DEBUG("D:No se pudo agregar la herramienta %s", toolname.c_str());
   if (ptoolbook_) {
      wxSize size = ptoolbook_->GetSize();
      REPORT_DEBUG("D:%s : (%d ; %d)",
    		  pTool->GetToolName().c_str(), size.GetWidth(), size.GetHeight());
   }
#endif

   return retval;
}

/**
 * Agrega un visualizador.
 * Setea la lista de elementos del contexto al visualizador.
 * Agrega a la ventana del contexto las herramientas del visualizador.
 * @return true si pudo agregar el visualizador.
 * @return false C.O.C.
 */
bool Context::AddViewer(suri::ViewerWidget *pViewer) {
   if (AddControl(pViewer, wxT("ID_VIEWER"))) {
      pViewer_ = pViewer;
      pViewer_->SetElementList(pElementList_);
      pDataViewManager_->GetViewcontextManager()->AddViewcontext(pViewer->GetViewcontextUuid(), pViewcontext_);
      viewcontextId_ = pViewer->GetViewcontextUuid();
      pViewcontext_->SetViewcontextId(viewcontextId_);
      pViewcontext_->SetViewcontextManager(pDataViewManager_->GetViewcontextManager());
      pTreeWidget_->SetViewcontextId(viewcontextId_);
      // obtengo la ventana de las herramientas del visualizador
      AddControl(pViewer->GetViewerTools(pToolWindow_),
                 XRCCTRL(*pToolWindow_,wxT("ID_CONATINERMID"),wxWindow),
                 suri::ControlArrangement(0, 0));
      return true;
   }
   return false;
}
/**
 * Inserta un elemento en el contexto.
 * Invoca al metodo abstracto AddElement para que se cree
 * el elemento. En caso de exito agrega al xml del elemento el nodo copyright y
 * el nodo descripcion, ambos con valores default obtenidos de la configuracion.
 * @param[in]	UrlElement: url del elemento a insertar.
 * @return puntero al elemento insertado o NULL.
 */
suri::Element *Context::InsertElement(const wxString& UrlElement) {
   suri::Element *pelement = AddElement(UrlElement, true);
   if (pelement) {
      std::string value = "";
      if (pelement->GetCopyRight().IsEmpty()) {
         value = suri::Configuration::GetParameter("pat_default_copyright", wxT(""));
         pelement->SetCopyRight(value.c_str());
      }
      if (pelement->GetDescription().IsEmpty()) {
         value = suri::Configuration::GetParameter("pat_default_description", wxT(""));
         pelement->SetDescription(value.c_str());
      }
      pelement->SetChanged();
      pelement->SendViewerUpdate();
   }
   return pelement;
}

/**
 * Objetivo: Agrega un elemento a la lista del contexto.
 * @param[in]	pNodeElement: nodo elemento del elemento a agregar al contexto.
 * @param[in]	ManuallyAdd: Se pasa como parametro al overload AddElement
 * @return devuelve el elemtno insertado.
 * @return NULL en caso de no poder agregar el elemento.
 */
suri::Element *Context::AddElement(const wxXmlNode *pNodeElement,
                                   const bool &ManuallyAdd) {
   //obtengo el path del archivo.
   wxXmlNode *pchild = pNodeElement->GetChildren();
   while (pchild && pchild->GetName().Cmp(URL_NODE) != 0)
      pchild = pchild->GetNext();

   //Genero el elemento de contexto y lo agrego a la lista.
	suri::Element *pelement = AddElement(pchild->GetNodeContent(), ManuallyAdd,
			pNodeElement);
   if (!pelement)		//significa que el contexto no pudo interpretar el elemento.
      return NULL;
   //obtengo el elemento de la lista y le reemplazo el nodo raiz.
#ifdef __UNUSED_CODE__
   suri::Element *pelementnew = pElementList_->GetElement(pelement);
   if (!pelementnew) return NULL;
   pelementnew->AddNode(NULL, new wxXmlNode(*pNodeElement), true);
   //al cambiar todo el nodo, debo enviar un update
   pelementnew->SetChanged();
   pelementnew->SendViewerUpdate();
   return pelementnew;
#endif
   return pelement;
}

/**
 * Cierra todos los  elementos
 */
void Context::CloseElements() {
   suri::ViewcontextManagerInterface* pvmgr =
         pDataViewManager_->GetViewcontextManager();
   pvmgr->SetContext(this);
   // Se hace downcasting para no manchar la interfaz.
   suri::DefaultViewcontextManager *pviewcontextmgr =
         dynamic_cast<suri::DefaultViewcontextManager *>(pvmgr);
   suri::ViewcontextInterface* pviewcontext = pviewcontextmgr->GetViewcontext(viewcontextId_);
   if (pviewcontext) {
      suri::LayerList::ElementListType::iterator first =
            pviewcontext->GetLayerList()->GetElementIterator(), last =
            pviewcontext->GetLayerList()->GetElementIterator(false);
      pviewcontextmgr->CloseElements(first, last);
      pviewcontext->RemoveAllLayers();
   }
}

//! Necesita un GetProperties especial porque tiene las listas
/**
 * Devuelve un nodo xml del contexto.
 * @return puntero a un nodo Contexto del xml.
 */
wxXmlNode *Context::GetProperties() {
   wxXmlNode *pnode = pElementList_->GetProperties();
   // creo un nodo copiado del de este elemento
   wxXmlNode *proot = new wxXmlNode(*pXmlProperties_);
   if (pnode) {
      wxXmlNode *pchild = pnode->GetChildren();
      pnode->RemoveChild(pchild);
      while (pchild) {
         AddNode(proot, pchild, false);  //dejar con Replace=True
         pchild = pnode->GetChildren();
         pnode->RemoveChild(pchild);
      }
   }
   // borro el nodo copiado de la lista
   delete pnode;
   // paso la copia con los elementos
   return proot;
}

/**
 * Evento UIUpdate que se ejecuta cuando la cola de eventos de usuario (que
 * maneja Wx) esta vacia. Des/Habilita las herramientas del contexto segun el
 * estado de la herramienta actual.
 * @param[in]	Evento: evento.
 */
void Context::OnUIUpdate(wxUpdateUIEvent &Event) {
   Event.Skip(false);
#ifdef __WXGTK__
   // busca detectar si esta inactivo debido a que algun padre lo esta
   bool isenabled = true;
   wxWindow *ptemp = pToolWindow_;
   while (isenabled && ptemp) {
      ptemp = ptemp->GetParent();
      if (ptemp) isenabled = ptemp->IsEnabled();
      if (ptemp->IsTopLevel()) ptemp = NULL;
   }
   if (!isenabled) return;
#endif
   bool enabled = pCurrentTool_ ? pCurrentTool_->IsEnabled() : false;
   //Solo (des)habilita el toolbar si esta en estado contrario
   if (pCurrentTool_->GetToolWindow()->IsEnabled() != enabled) {
      pCurrentTool_->GetToolWindow()->Enable(enabled);
      Event.Enable(enabled);
   }
}

/**
 *
 * @return string con las extensiones de los archivos soportados por el contexto.
 * \note el string devuelto debe cumplir con el formato descripto en pagina de
 * ayuda de wxDialog.
 */
std::string Context::GetSupportedFilesExt() {
   return supportedFilesExt_;
}

/**
 * Retorna el widget que se usa para despelgar los elementos
 * \note Este metodo solo existe hasta que LayerList soporte grupos. Solo
 * usar el LayerTreeWidget con el proposito de agregar/sacar grupos de la
 * interfaz grafica.
 * @return suri::LayerTreeWidget* que permite agregar/sacar grupos
 */
suri::LayerTreeWidget* Context::GetLayerTreeWidget() {
   return pLayersTree_;
}

/** Retorna el tipo de contexto */
Context::ContextType Context::GetContextType() const {
   return typeContext_;
}

/**
 * Metodo que responde a los eventos de click sobre el boton de Help.
 * Despliega la ventana de ayuda en la pagina 0(cero).
 */
void Context::OnButtonHelp() {
   // TODO(Federico - TCK #4479): Es necesario verificar por que estaba comentado el unused code
   ApplicationWindowManager::Instance().GetHelp(0);
#if __UNUSED_CODE__
   wxGetApp().GetHelp()->Display(0);
   wxGetApp().GetHelp()->SetFrameParameters(_(caption_HELP_s),wxSize(1024,600));
#endif
}

/**
 * Evento que se dispara justo antes de cambiar de herramienta.
 * Frena el cambio de herramienta si todavia no se termino de ejecutar el metodo
 * OnToolSelected.
 *
 * Previene codigo reentrante durante una renderizacion disparada por el cambio
 * de herramienta, que, durante el bucle llama a wxYield() permitiendo un nuevo
 * cambio de herramienta por procesar los eventos de wx.
 * @param[in]	Event: evento de cambio de pagina, utilizado para vetar el cambio.
 */
void Context::OnToolSelecting(wxNotebookEvent &Event) {
   if (isChangingTool_) {
      Event.Veto();
      currentChoicebookSelection_ = Event.GetOldSelection();
   }
}

/**
 * Evento que se dispara al seleccionar una herramienta.
 *
 * Activa la herramienta seleccionada y desactiva el resto que tienen ventana
 * asociada.
 * \note Marca la ejecucion con el flag isChangingTool para evitar codigo
 * reentrante, ver OnToolSelecting.
 * @param[in] Event: evento de cambio de pagina.
 */
void Context::OnToolSelected(wxNotebookEvent &Event) {
   if (Event.IsAllowed()) {
      isChangingTool_ = true;
      int sel = Event.GetSelection();
      std::multimap<int, suri::Tool*>::iterator it = toolMap_.begin();
      // recorro las herramientas
      while (it != toolMap_.end()) {
         // activo la seleccionada
         if ((*it).first == sel) {
            REPORT_DEBUG("D:Herramienta seleccionada %s(%d)",
                         wxT((*it).second->GetToolName()).c_str(), sel);
            (*it).second->SetActive();
            pCurrentTool_ = (*it).second;
            currentTool_ = sel;
         } else
         // las otras, si no son generales (-1) las desactivo
         if (sel != -1) {
            (*it).second->SetActive(false);
         }
         it++;
      }
      isChangingTool_ = false;
   } else  //Event not Allowed
   {
      if (currentChoicebookSelection_ != -1)
         ptoolbook_->ChangeSelection(currentChoicebookSelection_);
   }
}

/**
 * Verifica, comparando URLs, que el elemento a agregar no exista en la lista.
 * @param[in]	Url: path absoluto del elemento a insertar.
 * @return	si puede insertar el elemento: true; caso contrario: false.
 */
bool Context::CanInsertElement(const wxString &Url) {
   wxFileName fntoinsert(Url);
   for (int i = 0; i < pElementList_->GetElementCount(); i++) {
      wxFileName fninlist(pElementList_->GetElement(i)->GetUrl());
      if (fntoinsert.GetFullPath().Lower().Cmp(fninlist.GetFullPath().Lower()) == 0)
         return false;
   }
   return true;
}


/**
 * Retorna el tipo de visualizador de contexto (V2D, V3D, Multimedia, Html) dependiendo del contexto.
 */
suri::ViewcontextInterface::ViewcontextType Context::GetViewcontextType() {
	switch (typeContext_) {
	case ContextGenericType:
		break;
	case Context2DType:
		return suri::ViewcontextInterface::V2D;
		break;
	case ContextMapType:
		return suri::ViewcontextInterface::Map;
		break;
	case Context3DType:
		return suri::ViewcontextInterface::V3D;
		break;
	case ContextDocumentsType:
		return suri::ViewcontextInterface::Documents;
		break;
	case ContextMultimediasType:
		return suri::ViewcontextInterface::Multimedia;
		break;
	case ContextActivitiesType:
		return suri::ViewcontextInterface::Exercises;
		break;
	case ContextErrorType:
		break;
	default:
		break;
	}
	return suri::ViewcontextInterface::V2D;
}

/** Obtiene el viewport que corresponde con el id pasado por parametro */
suri::ViewportInterface* Context::GetViewport(
      const suri::SuriObject::UuidType &ViewportId) const {
      return pViewer_->GetId().compare(ViewportId) == 0 ? pViewer_ : NULL;
}

/** Devuelve el Viewport seleccionado */
suri::ViewportInterface* Context::GetSelectedViewport() const {
   return pViewer_;
}

/** Devuelve un vector con el id del viewer **/
std::vector<suri::SuriObject::UuidType> Context::GetViewportIds() const {
   std::vector< suri::SuriObject::UuidType> vector;
   vector.push_back(pViewer_->GetId());
   return vector;
}

/** Actualiza de forma manual la lista **/
void Context::ForceListUpdate() {
   if (pTreeWidget_) {
      pTreeWidget_->UpgradeControlContent();
      pTreeWidget_->ForceRefresh();
   }
}

/** Actualiza el visualizador de forma manual*/
void Context::ForceViewerUpdate() {
   if (pViewer_)
      pViewer_->GetWindow()->Refresh();
}
