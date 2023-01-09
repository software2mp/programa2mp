/*! \file ApplicationContextManager.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2012-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef APPLICATIONCONTEXTMANAGER_H_
#define APPLICATIONCONTEXTMANAGER_H_

// Includes Estandar
#include <vector>

// Includes Suri
#include "suri/ViewportManagerInterface.h"
#include "suri/ToolManagerInterface.h"
#include "suri/GuiInterface.h"
#include "suri/SuriObject.h"
#include "suri/ViewcontextInterface.h"
#include "suri/Subject.h"
#include "suri/NotificationInterface.h"
#include "suri/LayerInterface.h"

// Includes Wx

// Includes App

// Defines

// forwards
class Context;
class ContextManager;
class ViewerWidget;

/**
 * Manager de Contextos de la Aplicacion
 * \TODO(Sebastian - TCK #852): Analizar eliminacion de ApplicationContextManager
 */
class ApplicationContextManager : public suri::ViewportManagerInterface,
                                  public suri::ToolManagerInterface {
public:
   /** Constructor */
   ApplicationContextManager();
   /** Destructor */
   virtual ~ApplicationContextManager();
   /** Seter del GUI */
   void SetGui(suri::GuiInterface *pGui);

   suri::GuiInterface* GetGui();

   /** Crea un Viewport del tipo especificado y lo agrega a los manejados */
   virtual suri::SuriObject::UuidType CreateViewport(
         const suri::SuriObject::ClassIdType &ViewportType);
   /** Obtiene un listado con los Ids de Viewports que esta manejando */
   virtual std::vector<suri::SuriObject::UuidType> GetViewportIds() const;
   /** Selecciona un Viewport segun su Id */
   virtual void SelectViewport(const suri::SuriObject::UuidType &ViewportId);
   /** Obtiene la instancia del Viewport previamente seleccionado */
   virtual suri::ViewportInterface* GetSelectedViewport() const;
   /** Obtiene la instancia de un viewport en funcion de su Id */
   virtual suri::ViewportInterface* GetViewport(const suri::SuriObject::UuidType &ViewportId) const;
   /** Activa/Desactiva un viewport */
   virtual void SetViewportActive(const suri::SuriObject::UuidType &ViewportId, bool Active = true);
   /** Indica si el Viewport esta activado */
   virtual bool IsViewportActive(const suri::SuriObject::UuidType &ViewportId) const;
   /** Agrega una herramienta */
   virtual void AddTool(suri::ToolInterface *pTool);
   /** Remueve una herramienta */
   virtual void RemoveTool();

private:
   /** GUI del Menu de Aplicacion */
   suri::GuiInterface *pApplicationMenuGui_;
};

#endif /* APPLICATIONCONTEXTMANAGER_H_ */
