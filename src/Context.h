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

#ifndef CONTEXT_H_
#define CONTEXT_H_

//Includes standard
#include <map>
#include <string>
#include <vector>

//Includes Suri
#include "suri/Widget.h"
#include "suri/XmlElement.h"
#include "suri/Element.h"
#include "suri/LayerList.h"
#include "suri/FactoryHelpers.h"
#include "suri/LayerTreeWidget.h"
#include "suri/SuriObject.h"
#include "suri/ViewcontextInterface.h"
#include "suri/DataViewManager.h"

//Includes Wx
#include "wx/choicebk.h"

//Includes App

//Defines
// #define __USE_LAYER_TREE_WIDGET__

// Forwards Suri
namespace suri {
class ViewerWidget;
class Tool;
class DataViewManager;
class HtmlTreeWidget;
}

// Forwards Wx
class wxNotebookEvent;

DECLARE_EVENT_OBJECT(ContextEvent);

//! Clase abstracta para los contextos
/**
 * Esta clase es la base para crear todos los contextos de la applicacion,
 * Context2D, Context3D, ContextHtml.
 * Posee:
 * 	- Lista de elementos cargados en el contexto.
 * 	- Visualizador para poder desplegar los elementos.
 * 	- Mapa de las herramientas que maneja.
 * Permite:
 * 	- agregar/eliminar herramientas.
 * 	- agregar/eliminar elementos (sin repeticion al agregar).
 * 	- obtener un xml con los elementos cargados.
 * Esta clase define un tipo de contexto, mediante el cual identifica a los
 * distintos contextos ya que existen contextos que son del mismo tipo(Por
 * ejemplo: Contexto2D, ContextoMapa).
 */
class Context : public suri::Widget, public suri::XmlElement {
HAS_FACTORY(Context,std::string,wxWindow*);
public:

   typedef enum {
		ContextGenericType,
		Context2DType,
		Context3DType,
		ContextMultimediasType,
		ContextHtml,
		ContextMapType,
		ContextDocumentsType,
		ContextActivitiesType,
		ContextErrorType
	} ContextType;

   FRIEND_EVENT_OBJECT(ContextEvent);

   //! Ctor.
	Context(wxWindow *pParent = NULL, suri::DataViewManager* pDataViewManager =
			NULL, const wxString &ContextType = wxEmptyString);
   //! Dtor.
   virtual ~Context();
   //! Crea la ventana
   virtual bool CreateToolWindow();
   //! Agrega una herramienta al contexto
   bool AddTool(suri::Tool *pTool);
   //! Agrega un visualizador
   bool AddViewer(suri::ViewerWidget *pViewer);
   //! Inserta un elemento
   suri::Element *InsertElement(const wxString& Filename);
   //! Agrega un elemento al contexto
   virtual suri::Element *AddElement(const wxXmlNode *pElement,
                                     const bool &ManuallyAdd = false);
   //! Cierra todos los  elementos
   virtual void CloseElements();
   //! Necesita un GetProperties especial porque tiene las listas
   virtual wxXmlNode *GetProperties();
   //! Devuelve la lista de elementos del contexto
   suri::LayerList *GetElementList() {
      return pViewcontext_ ? pViewcontext_->GetLayerList() : pElementList_;
   }
   ;
   //! Evento UIUpdate
   void OnUIUpdate(wxUpdateUIEvent &Event);
   //! Devuelve lista de extensiones de archivos soportados.
   std::string GetSupportedFilesExt();
   /** Retorna el tipo de contexto */
   ContextType GetContextType() const;
   /** Retorna el widget que se usa para despelgar los elementos */
   suri::LayerTreeWidget* GetLayerTreeWidget();
   // TODO(Gabriel - TCK #852): context de simur diferente a context de sopi
   void SelectViewcontext(const suri::SuriObject::UuidType &ViewcontextId) {}
   suri::ViewcontextInterface * GetViewcontext(
         const suri::SuriObject::UuidType &ViewcontextId) const {
      if (pViewcontext_
            && pViewcontext_->GetViewcontextId().compare(ViewcontextId) == 0) {
         return pViewcontext_;
      }
      return NULL;
   }
   suri::ViewcontextInterface *GetSelectedViewcontext() {
      return pViewcontext_;
   }
   std::vector< suri::SuriObject::UuidType > GetViewcontextIds() const {
      std::vector< suri::SuriObject::UuidType> vector;
      vector.push_back(pViewcontext_->GetViewcontextId());
      return vector;
   }
   /** Obtiene el viewport que corresponde con el id pasado por parametro */
   virtual suri::ViewportInterface* GetViewport(
         const suri::SuriObject::UuidType &ViewportId) const;
   /** Devuelve el Viewport seleccionado */
   virtual suri::ViewportInterface* GetSelectedViewport() const;
   /** Devuelve un vector con el id del viewer **/
   std::vector<suri::SuriObject::UuidType> GetViewportIds() const;
   /** Actualiza de forma manual la lista **/
   void ForceListUpdate();
   /** Actualiza el visualizador de forma manual*/
   void ForceViewerUpdate();
   /** Retorna el tipo de visualizador de contexto (V2D, V3D, Multimedia, Html) dependiendo del contexto. */
   suri::ViewcontextInterface::ViewcontextType GetViewcontextType();


protected:
   //! Agrega un elemento al contexto
   virtual suri::Element *AddElement(const wxString& Filename,
                                     const bool &ManuallyAdd, const wxXmlNode* pElementNode = NULL)=0;
   //! Evento de Help
   virtual void OnButtonHelp();
   //! Evento previo a cambiar de herramienta
   void OnToolSelecting(wxNotebookEvent &Event);
   //! Evento de seleccionar una herramienta
   void OnToolSelected(wxNotebookEvent &Event);
   //! Verifica que el elemento a agregar no exista en la lista.
   bool CanInsertElement(const wxString &Url);
//---- Propiedades ----
   suri::LayerList *pElementList_;	///< Listado de los elementos del contexto
   suri::ViewerWidget *pViewer_;  ///< Visualizador de elementos
   suri::DataViewManager* pDataViewManager_;
   ContextType typeContext_;  ///< id de contexto para identificar el contexto cuando se muestra help
   std::string supportedFilesExt_;  ///< contiene extensiones de archivos soportados por cada contexto.
   suri::ViewcontextInterface* pViewcontext_; ///< Visualizador de contexto.
   suri::SuriObject::UuidType viewcontextId_; ///< Id que identifica cada contexto.
   /** Widget que despliega las capas del contexto en una estructura de arbol **/
   suri::HtmlTreeWidget* pTreeWidget_;

private:
   std::multimap<int, suri::Tool*> toolMap_;	///< Herramientas del contexto
   std::vector<suri::Tool*> canDelTools_;
   int currentTool_;  ///< Indice de la herramienta seleccionada
   suri::Tool *pCurrentTool_;  ///< Herramienta actual
   EVENT_OBJECT_PROPERTY(ContextEvent);
   ///< propiedad que previene el assert al buscar el pulldown
   bool preventAssert_;
   ///< Choicebook de herramientas del contexto
   wxChoicebook *ptoolbook_;
   ///< Seleccion actual para volver a ella en caso que se vete el cambio de herramienta
   int currentChoicebookSelection_;
   ///< Flag que indica si se esta cambiando de herramienta. En este caso se veta un nuevo cambio
   bool isChangingTool_;
   /*! Widget que despliega los elementos del contexto */
   suri::LayerTreeWidget *pLayersTree_;
   /** Metodo auxiliar que inicializa la lista de capas correspondiente
    *  al visualizador activo (en caso de Simur es unico por contexto)
    */
   void InitializeLayerListWidget();
};

#endif /*CONTEXT_H_*/
