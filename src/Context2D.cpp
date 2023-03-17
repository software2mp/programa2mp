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

//Includes standard
#ifdef __DEBUG__
#include <iostream>
#include <time.h>
#endif

#include <vector>

//Includes Suri
#include "Context2D.h"
#include "suri/Viewer2D.h"
#include "suri/LayerList.h"
#include "suri/ViewerWidget.h"
#include "suri/DataTypes.h"
#include "suri/ActivationLogic.h"
#include "suri/ActiveRasterWorldExtentManager.h"
#include "suri/VectorEditor.h"
#include "suri/NavigationWidget.h"
#include "suri/LatitudeLongitudeInputWidget.h"
#include "suri/AnimationTool.h"
#include "suri/BrightnessTool.h"
#include "suri/DrawTool.h"
#include "suri/GpsTool.h"
#include "suri/MapTool.h"
#include "suri/PixelInfoTool.h"
#include "suri/LibraryManagerFactory.h"
#include "suri/RasterElement.h"
#include "suri/ViewcontextInterface.h"
#include "MeassureWidget.h"
#include "CoordinatesNavigatorPart.h"

//Includes Wx
#include "wx/wx.h"
#include "wx/imaglist.h"
#include "wx/listbook.h"
#include "wx/xrc/xmlres.h"

//Includes App
#include "messages.h"
#include "SingleRasterElementActivationLogic.h"
#include "resources.h"

//Defines

#define DEFAULT_WKT_STYLE "VECTORSTYLE[3,\"Poligono rojo\",BRUSH[\"Suri-brush\",0,COLOR[0,0,0,255]],PEN[\"Suri-pen\",1,COLOR[32,89,108,255],WIDTH[1]],SYMBOL[\"Suri-symbol\",2,COLOR[63,113,154,255],SIZE[2]]]"

AUTO_REGISTER_CLASS(Context, Context2D, 1, wxWindow*)

// ----------------------- CLASE DE ELEMENTO TIPO MAPA ------------------------

START_IMPLEMENT_EVENT_OBJECT(Context2DEvent,Context2D)END_IMPLEMENT_EVENT_OBJECT

/**
 * Constructor.
 * @param[in]   pParent: puntero a ventana padre.
 * @param[in] pDataViewManager puntero a visualizador del contexto.
 * @param[in]   Type: tipo de contexto.
 */
Context2D::Context2D(wxWindow *pParent, suri::DataViewManager* pDataViewManager, ContextType Type) :
      Context(pParent, pDataViewManager, wxT(TYPE_PROPERTY_VALUE_RASTER)),
      NEW_EVENT_OBJECT(Context2DEvent)  //No se traduce, contextType
{
   pVectorEditor_ = new suri::VectorEditor();
   windowTitle_ = _(caption_RASTER_CONTEXT);
   typeContext_ = Type;
   supportedFilesExt_ = wxString::Format("%s%s%s",
                                         (filter_CONTEXT_IMAGE_FILES_LOAD).c_str(),
                                         filter_FORMAT_SEPARATOR,
                                         _(filter_ALL_FILES_LOAD)).c_str();
   pDataViewManager_ = pDataViewManager;
}

/**
 * Dtor.
 * Destruye el objeto de eventos.
 */
Context2D::~Context2D() {
   DELETE_EVENT_OBJECT
   ;
}

/**
 * Crea/carga la ventana del Contexto.
 * A la ventana creada por la clase base le agrega el visor del contexto y las herramientas del Contexto
 * Herramientas de Imagen:
 *  - BrightnessTool
 *  - MapTool
 *  - AnimationTool
 *  - MeassureTool
 * Herramientas comunes (Imagen y Mapa):
 *  - DrawTool
 *  - PixelInfo (no posee ventana asociada)
 */
bool Context2D::CreateToolWindow() {
	Context::CreateToolWindow();

   // Agrego el visualizador
   suri::Viewer2D *pviewer = new suri::Viewer2D(pToolWindow_, pDataViewManager_);
   AddViewer(pviewer);
   // Cargo un extent manager que forme el mundo con el join de rasters activos
   pviewer->SetWorldExtentManager(
         new suri::ActiveRasterWorldExtentManager(pElementList_, pviewer->GetWorld()));
   pElementList_->SetActivationLogic(
         new SingleRasterElementActivationLogic(
               pviewer, message_RASTER_IMAGE_MUST_BE_DISPLAYED));  //no se traduce. Despues se usa en SHOW_

   // Se muestra boton que importa capas wms.
   // Nota: Si se desea ocultar el boton en contexto 'Mapas' mover linea dentro de if
   GET_CONTROL(*pToolWindow_, "ID_BUTTON_ADD_WMS_ELEMENT", wxBitmapButton)->Show();

   // Agrego las herramientas del contexto Raster
   if (typeContext_ == Context2DType || typeContext_ == ContextMapType) {
      AddTool(new suri::BrightnessTool(pToolWindow_));
      std::vector<int> vecgroup;
      vecgroup.push_back(VectorPosition);
      vecgroup.push_back(RasterPosition);
      if (typeContext_ == Context2DType) {
         AddTool(new suri::MapTool(pviewer->GetWorld(), vecgroup, pToolWindow_));
      }
      AddTool(
            new suri::AnimationTool(
                  XRCCTRL(*pToolWindow_,wxT("ID_LAYERS"),wxWindow),
                  XRCCTRL(*pToolWindow_,wxT("ID_CONATINERLEFT"),wxWindow),
                  RasterPosition, pToolWindow_, pDataViewManager_, pTreeWidget_));
      if (typeContext_ == Context2DType) {
         AddTool(
               new suri::GpsTool(
                     VectorPosition, pVectorEditor_, pDataViewManager_, viewcontextId_,
                     XRCCTRL(*pToolWindow_, wxT("ID_CONATINERRIGHT"), wxWindow)));
      }

   }
   // Herramientas comunes a ambos contextos
   AddTool(
         new suri::DrawTool(AnotationPosition, VectorPosition, pVectorEditor_, pDataViewManager_,
        		 viewcontextId_, XRCCTRL(*pToolWindow_,wxT("ID_CONATINERRIGHT"),wxWindow)));
   AddTool(new suri::PixelInfoTool(pviewer->GetWorld()));

   //Agrego navigator a layer
   //Crea Widget que permite navegar en latitud y longitud
#ifdef __UNUSED_CODE__
   suri::LatitudeLongitudeInputWidget* latlonginputwidget =
         new suri::LatitudeLongitudeInputWidget();
   suri::NavigationWidget* pnavigationwidget = new suri::NavigationWidget(
         XRCCTRL(*pToolWindow_, "ID_SPLITTERLEFT", wxWindow), pviewer,
         latlonginputwidget);
#else
   suri::CoordinatesNavigatorPart* pcoordinput =
         new suri::CoordinatesNavigatorPart(NULL, false);
   pcoordinput->SetDataViewManager(pDataViewManager_);
   suri::NavigationWidget* pnavigationwidget = new suri::NavigationWidget(
         XRCCTRL(*pToolWindow_,"ID_SPLITTERLEFT", wxWindow), pviewer ,
         pcoordinput, pDataViewManager_);

#endif
   pnavigationwidget->Hide();
   pviewer->GetNavigator()->SetWidget(pnavigationwidget);

   // NavigationWidget se mostrara debajo del LayerListWidget
   AddControl(pviewer->GetNavigator()->GetWidget(), "ID_SPLITTERLEFT");
   Hide(wxT("ID_SPLITTERLEFT"));

   // Herramienta de medicion rapida
   suri::MeassureWidget* pquickmeasure = new suri::MeassureWidget(
         XRCCTRL(*pToolWindow_, "ID_MEASSURE_TOOL", wxWindow), pviewer, VectorPosition,
         pVectorEditor_, this, pDataViewManager_);

   AddControl(pquickmeasure, wxT("ID_MEASSURE_TOOL"));
   pviewer->SetMeasureWidget(pquickmeasure);

   Hide(wxT("ID_MEASSURE_TOOL"));

   pViewer_->SetViewcontext(pViewcontext_);
   if (!pDataViewManager_->GetViewcontextManager()->GetSelectedViewcontext()) {
      pDataViewManager_->GetViewcontextManager()->SelectViewcontext(viewcontextId_);
   }
   return true;
}

/**
 * Retorna el icono del contexto.
 *   @param[in]   X: sin utlizar
 *   @param[in]   Y: sin utlizar
 *   @param[out]   ToolBitmap: bitmap con el icono del contexto.
 */
void Context2D::GetWindowIcon(wxBitmap& ToolBitmap, int X, int Y) const {
   GET_BITMAP_RESOURCE(wxT(icon_RASTER_CONTEXT), ToolBitmap);
}

/**
 * Agrega un elemento a la lista del contexto.
 * Para poder crear los distintos tipos de elemento que admite el contexto,
 * intenta crear todos los tipos de elemntos hasta que alguno machee.
 * Valida que no se inserte un elemento repetido.
 * @param[in] UrlElement url del elemento a agregar.
 * @param[in] ManuallyAdd No se utiliza en este contexto.
 * @return puntero al elemento creado.
 * @return NULL en caso de que el elemento no sea valido para el contexto.
 * \attention: el puntero devuelto no debe ser eliminado ya que la lista de
 * elementos(LayerList) se hace cargo del mismo.
 */
suri::Element *Context2D::AddElement(const wxString& UrlElement,
                                     const bool &ManuallyAdd,
                                     const wxXmlNode* pElementNode) {
   // verifico que el elemento no se encuentre en la lista.
   if (!CanInsertElement(UrlElement)) return NULL;

   LayerPositionType elementposition[] = { Context2D::RasterPosition,
                                           Context2D::RasterPosition,
                                           Context2D::AnotationPosition,
                                           Context2D::VectorPosition,
                                           Context2D::VectorPosition };
   const char *pelementtypes[] = { "WxsElement", "RasterElement", "AnotationElement",
                                   "HotLinkVectorElement", "VectorElement" };
   size_t i = 0;
   LayerPositionType pos = Context2D::DefaultPosition;
   suri::Element *pelement = NULL;
   do {
      pelement = suri::Element::Create(pelementtypes[i], UrlElement.c_str());
      pos = elementposition[i++];
   } while (pelement == NULL && i < sizeof(pelementtypes) / sizeof(pelementtypes[0])
         && i < sizeof(elementposition) / sizeof(elementposition[0]));

   if (!pelement)
      return NULL;
   wxFileName filename(UrlElement);
   if (pelement->GetName().IsEmpty())
      pelement->SetName(filename.GetName());
   if (pElementNode) {
      wxXmlNode* pfilenode = new wxXmlNode(*pelement->GetNode(FILE_NODE));
      //obtengo el elemento de la lista y le reemplazo el nodo raiz.
      pelement->AddNode(NULL, new wxXmlNode(*pElementNode), true);
      if (dynamic_cast<suri::VectorElement*>(pelement)) {
         UpdateXMLStyleNode(pelement);
      } else if (dynamic_cast<suri::RasterElement*>(pelement)) {
         UpdateXMLCacheNode(pelement);
         pelement->GetNode()->InsertChild(pfilenode, NULL);
      }
   }
   const char *pdstypes[] = { "RasterDatasource", "VectorDatasource" };
   suri::DatasourceInterface* pdatasource = NULL;
   for (int i = 0; i < 2 && pdatasource == NULL; ++i) {
      pdatasource = suri::DatasourceInterface::Create(pdstypes[i], pelement);
   }
   if (pdatasource) {

      suri::VectorElement* pvelement = dynamic_cast<suri::VectorElement*>(pdatasource->GetElement());
      if ( pvelement ) { // Si es un vector.
         suri::Vector::VectorType vtype = pvelement->GetType();
         std::string strWkt = "";

         if ( vtype == suri::Vector::Polygon )
            strWkt = suri::Configuration::GetParameter("geo_polygon_style", "");
         else if ( vtype == suri::Vector::Line )
            strWkt = suri::Configuration::GetParameter("geo_line_style", "");
         else if ( vtype == suri::Vector::Point )
            strWkt = suri::Configuration::GetParameter("geo_point_style", "");

         if ( strWkt.empty())
            strWkt = DEFAULT_WKT_STYLE;

         pvelement->SetStyle(strWkt);
      }
      if (GetContextType() == ContextMapType) {
         pelement->SetIcon(icon_MAP_ELEMENT);
      }
      pDataViewManager_->GetDatasourceManager()->AddDatasourceWithNotification(
            pdatasource, viewcontextId_, static_cast<suri::ViewcontextInterface::ViewcontextType>(GetContextType()));
   }
   return pelement;
}

/* Actualiza el nodo estilo */
void Context2D::UpdateXMLStyleNode(suri::Element *pelement) {
   std::string path = std::string(
         RENDERIZATION_NODE) + NODE_SEPARATION_TOKEN + LAYERS_NODE;
   wxXmlNode *plsnode = pelement->GetNode(path);
   wxXmlNode* pchild = plsnode->GetChildren();
   if (pchild && (!pchild->GetNodeContent().empty())) {
      std::string wkt = pchild->GetNodeContent().c_str();
      pelement->AddNode(pchild, STYLE_NODE, wkt);
      pchild->SetContent("");
   }
}

/* Actualiza el nodo cache */
void Context2D::UpdateXMLCacheNode(suri::Element *pelement) {
   wxXmlNode *plsnode = pelement->GetNode(RENDERIZATION_NODE);
   std::string path = std::string(RENDERIZATION_NODE) + NODE_SEPARATION_TOKEN + BAND_COMBINATION_NODE;
   wxXmlNode *combinationnode = pelement->GetNode(path);
   path = std::string(
         RENDERIZATION_NODE) + NODE_SEPARATION_TOKEN + CANVAS_CACHE_NODE;
   if (!pelement->GetNode(path)) {
      wxXmlNode* pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(CANVAS_CACHE_NODE));
      plsnode->InsertChildAfter(pchild, combinationnode);
   }
}
